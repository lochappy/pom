
//////////////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify         //
// it under the terms of the version 3 of the GNU General Public License        //
// as published by the Free Software Foundation.                                //
//                                                                              //
// This program is distributed in the hope that it will be useful, but          //
// WITHOUT ANY WARRANTY; without even the implied warranty of                   //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             //
// General Public License for more details.                                     //
//                                                                              //
// You should have received a copy of the GNU General Public License            //
// along with this program. If not, see <http://www.gnu.org/licenses/>.         //
//                                                                              //
// Written by Francois Fleuret                                                  //
// (C) Ecole Polytechnique Federale de Lausanne                                 //
// Contact <pom@epfl.ch> for comments & bug reports                             //
// Modified by Matej Smid <smidm@cmp.felk.cvut.cz>                              //
//////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "pom_solver.h"
#include "global.h"

//////////////////////////////////////////////////////////////////////

POMSolver::POMSolver(Room *room)
{
    vNegs.resize(room->nb_cameras());
    vIINegs.resize(room->nb_cameras());
    vIINegView.resize(room->nb_cameras());
    for(int c = 0; c < room->nb_cameras(); c++) {
        vNegs[c].create(room->view_height(c),room->view_width(c),CV_64FC1);
    }
    global_difference.set(global_mu_image_density, global_sigma_image_density);
}

//////////////////////////////////////////////////////////////////////

void POMSolver::compute_average_images(const int camera, const Room *room, const cv::Mat &proba_absence)
{
    vNegs[camera].setTo(1.0);
    const double *proba_absence_ptr = (const double*)proba_absence.data;
    for(int n = 0; n < room->nb_positions(); n++) if(proba_absence_ptr[n] <= global_proba_ignored) {
        Rectangle *r = room->avatar(camera, n);
        if(r->visible)
            vNegs[camera](cv::Rect(r->xmin, r->ymin, r->getWidth()+1,r->getHeight()+1)) *= proba_absence_ptr[n];
    }
}

//////////////////////////////////////////////////////////////////////

void POMSolver::add_log_ratio(const int camera, const Room *room, const cv::Mat &proba_absence, cv::Mat &sum)
{
    // Computes the average on the complete picture

    compute_average_images(camera, room, proba_absence);

    const double s = vIINegs[camera].compute_sum(vNegs[camera]);
    const double sv = vIINegView[camera].compute_sum(vNegs[camera], room->get_view_CVMat(camera));

    const scalar_t noise_proba = 0.01; // 1% of the scene can remain unexplained
    const scalar_t average_surface = room->view_width(camera) * room->view_height(camera) * (1 + noise_proba) - s;
    const scalar_t average_diff = average_surface + sv;

    // Cycles throw all positions and adds the log likelihood ratio to
    // the total sum for each

    for(int i = 0; i < room->nb_positions(); i++) {
        Rectangle *r = room->avatar(camera, i);
        if(r->visible) {
            const scalar_t lambda = 1 - 1/proba_absence.at<double>(0,i);

            const scalar_t integral_neg =   vIINegs[camera].integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);

            const scalar_t average_surface_givpre = average_surface +          integral_neg;
            const scalar_t average_surface_givabs = average_surface + lambda * integral_neg;

            const scalar_t integral_neg_view =  vIINegView[camera].integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);

            const scalar_t average_diff_givpre = average_diff +           integral_neg - 2 * integral_neg_view;
            const scalar_t average_diff_givabs = average_diff + lambda * (integral_neg - 2 * integral_neg_view);

            const scalar_t log_mu0 = global_difference.log_proba(average_diff_givabs / average_surface_givabs);
            const scalar_t log_mu1 = global_difference.log_proba(average_diff_givpre / average_surface_givpre);

            sum.at<double>(0,i) += log_mu1 - log_mu0;

        }
    }
}

void POMSolver::solve(Room *room,
                      Vector<scalar_t> *prior,
                      Vector<scalar_t> *result_proba_presence,
                      int nb_frame,
                      char *convergence_file_format) {


    cv::Mat m_prior(1,prior->length(),CV_64FC1,prior->data());
    cv::Mat m_result_proba_presence(1,result_proba_presence->length(),CV_64FC1,result_proba_presence->data());

    cv::Mat m_log_prior_ratio(1,prior->length(),CV_64FC1);
    cv::Mat m_sum(1,room->nb_positions(),CV_64FC1);
    cv::Mat m_proba_absence(1,room->nb_positions(),CV_64FC1);


    cv::log(m_prior/(1.0 - m_prior),m_log_prior_ratio);
    m_proba_absence = 1.0 - m_prior;

    int nb_stab = 0;

    for(int it = 0; (nb_stab < global_nb_stable_error_for_convergence) && (it < global_max_nb_solver_iterations); it++) {

        m_sum.setTo(0.0);
        for(int c = 0; c < room->nb_cameras(); c++)
            add_log_ratio(c, room, m_proba_absence, m_sum);

        cv::Mat m_exp, m_err;
        cv::exp(m_log_prior_ratio+m_sum,m_exp);
        cv::Mat m_np = global_smoothing_coefficient*m_proba_absence + (1-global_smoothing_coefficient)/(1+m_exp);
        cv::absdiff(m_np,m_proba_absence,m_err);
        m_proba_absence = m_np;

        double e_max=0;
        cv::minMaxIdx(m_err,NULL,&e_max);
        if(e_max < global_error_max) nb_stab++; else nb_stab = 0;

        if(convergence_file_format) {
            char buffer[buffer_size];
            for(int p = 0; p < room->nb_positions(); p++) {
                m_result_proba_presence = 1 - m_proba_absence;
            }

            for(int c = 0; c < room->nb_cameras(); c++) {
                pomsprintf(buffer, buffer_size, convergence_file_format, c, nb_frame, it);
                cout << "Saving " << buffer << "\n"; cout.flush();
                room->save_stochastic_view(buffer, c, result_proba_presence);
            }
        }

    }

    m_result_proba_presence = 1 - m_proba_absence;
}
