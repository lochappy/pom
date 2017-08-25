
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

void POMSolver::compute_average_images(const int camera,
                                       const Room *room,
                                       Vector<scalar_t> *proba_absence) {

    vNegs[camera].setTo(1.0);
    for(int n = 0; n < room->nb_positions(); n++) if((*proba_absence)[n] <= global_proba_ignored) {
        Rectangle *r = room->avatar(camera, n);
        if(r->visible)
            vNegs[camera](cv::Rect(r->xmin, r->ymin, r->getWidth()+1,r->getHeight()+1)) *= (*proba_absence)[n];
    }

}

//////////////////////////////////////////////////////////////////////

void POMSolver::add_log_ratio(int camera,
                              Room *room,
                              Vector<scalar_t> *proba_absence,
                              Vector<scalar_t> *sum) {

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
            const scalar_t lambda = 1 - 1/(*proba_absence)[i];

            const scalar_t integral_neg =   vIINegs[camera].integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);

            const scalar_t average_surface_givpre = average_surface +          integral_neg;
            const scalar_t average_surface_givabs = average_surface + lambda * integral_neg;

            const scalar_t integral_neg_view =  vIINegView[camera].integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);

            const scalar_t average_diff_givpre = average_diff +           integral_neg - 2 * integral_neg_view;
            const scalar_t average_diff_givabs = average_diff + lambda * (integral_neg - 2 * integral_neg_view);

            const scalar_t log_mu0 = global_difference.log_proba(average_diff_givabs / average_surface_givabs);
            const scalar_t log_mu1 = global_difference.log_proba(average_diff_givpre / average_surface_givpre);

            (*sum)[i] += log_mu1 - log_mu0;

        }
    }
}

//void POMSolver::add_log_ratio(int camera,
//                              Room *room,
//                              Vector<scalar_t> *proba_absence,
//                              Vector<scalar_t> *sum) {

//    // Computes the average on the complete picture

//    compute_average_images(camera, room, proba_absence);

//    double s = ii_neg[camera]->compute_sum(neg[camera]);
//    double s_cv = vIINegs[camera].compute_sum(vNegs[camera]);
////    {//Small test by lochappy
////    cv::Scalar s_diff = cv::sum(cv::abs(ii_neg[camera]->getCvMat() - vIINegs[camera].getIntegralImage()));
////    std::cout << "s_diff = " << s_diff.val[0] <<std::endl;
////    }

//    double sv = ii_neg_view[camera]->compute_sum(neg[camera], room->get_view(camera));
//    double sv_cv = vIINegView[camera].compute_sum(vNegs[camera], room->get_view_CVMat(camera));

////    {//Small test by lochappy
////    cv::Scalar sv_diff = cv::sum(cv::abs(ii_neg_view[camera]->getCvMat() - vIINegView[camera].getIntegralImage()));
////    std::cout << "sv_diff = " << sv_diff.val[0] <<std::endl;
////    }


//    scalar_t noise_proba = 0.01; // 1% of the scene can remain unexplained
//    scalar_t average_surface = room->view_width(camera) * room->view_height(camera) * (1 + noise_proba) - s;
//    scalar_t average_diff = average_surface + sv;

//    // Cycles throw all positions and adds the log likelihood ratio to
//    // the total sum for each

//    for(int i = 0; i < room->nb_positions(); i++) {
//        Rectangle *r = room->avatar(camera, i);
//        if(r->visible) {
//            scalar_t lambda = 1 - 1/(*proba_absence)[i];

//            scalar_t integral_neg = ii_neg[camera]->integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);
//            scalar_t integral_neg_cv = vIINegs[camera].integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);
//            {//Small test by lochappy
//                std::cout << "integral_neg_diff = " << std::abs(integral_neg_cv - integral_neg) <<std::endl;
//            }

//            scalar_t average_surface_givpre = average_surface +          integral_neg;
//            scalar_t average_surface_givabs = average_surface + lambda * integral_neg;

//            scalar_t integral_neg_view = ii_neg_view[camera]->integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);
//            scalar_t integral_neg_view_cv = vIINegView[camera].integral(r->xmin, r->ymin, r->xmax + 1, r->ymax + 1);
//            {//Small test by lochappy
//                std::cout << "integral_neg_view_diff = " << std::abs(integral_neg_view_cv - integral_neg_view) <<std::endl;
//            }
//            scalar_t average_diff_givpre = average_diff +           integral_neg - 2 * integral_neg_view;
//            scalar_t average_diff_givabs = average_diff + lambda * (integral_neg - 2 * integral_neg_view);

//            scalar_t log_mu0 = global_difference.log_proba(average_diff_givabs / average_surface_givabs);
//            scalar_t log_mu1 = global_difference.log_proba(average_diff_givpre / average_surface_givpre);

//            (*sum)[i] += log_mu1 - log_mu0;

//        }
//    }
//}

void POMSolver::solve(Room *room,
                      Vector<scalar_t> *prior,
                      Vector<scalar_t> *result_proba_presence,
                      int nb_frame,
                      char *convergence_file_format) {


    cv::Mat m_prior(1,prior->length(),CV_64FC1,prior->data());

    Vector<scalar_t> log_prior_ratio(prior->length());
    Vector<scalar_t> sum(room->nb_positions());
    Vector<scalar_t> proba_absence(room->nb_positions());

    cv::Mat m_log_prior_ratio(1,prior->length(),CV_64FC1);
    cv::Mat m_sum(1,room->nb_positions(),CV_64FC1);
    cv::Mat m_proba_absence(1,room->nb_positions(),CV_64FC1);


    cv::log(m_prior/(1.0 - m_prior),m_log_prior_ratio);
    m_proba_absence = 1.0 - m_prior;

    for(int i  = 0; i < room->nb_positions(); i++) {
        log_prior_ratio[i] = log((*prior)[i]/(1 - (*prior)[i]));
        proba_absence[i] = 1 - (*prior)[i];
    }

//    {//Small test by lochappy
//        double log_prior_ratio_diff=0,proba_absence_diff = 0;
//        for(int i  = 0; i < room->nb_positions(); i++) {
//            log_prior_ratio_diff += std::abs(m_log_prior_ratio.at<double>(0,i) - log_prior_ratio[i]);
//            proba_absence_diff += std::abs(m_proba_absence.at<double>(0,i) - proba_absence[i]);
//        }
//        std::cout<< "log_prior_ratio_diff = " << log_prior_ratio_diff
//                 << "; proba_absence_diff = " << proba_absence_diff << std::endl <<std::flush;
//    }

    int nb_stab = 0;

    for(int it = 0; (nb_stab < global_nb_stable_error_for_convergence) && (it < global_max_nb_solver_iterations); it++) {

        sum.clear();
        for(int c = 0; c < room->nb_cameras(); c++)
            add_log_ratio(c, room, &proba_absence, &sum);

        scalar_t e = 0;
        for(int i = 0; i < room->nb_positions(); i++) {
            scalar_t np = global_smoothing_coefficient * proba_absence[i] +
                    (1 - global_smoothing_coefficient) / (1 + exp(log_prior_ratio[i] + sum[i]));
            if(abs(proba_absence[i] - np) > e) e = abs(proba_absence[i] - np);
            proba_absence[i] = np;
        }

        if(e < global_error_max) nb_stab++; else nb_stab = 0;

        if(convergence_file_format) {
            char buffer[buffer_size];
            for(int p = 0; p < room->nb_positions(); p++) {
                (*result_proba_presence)[p] = 1 - proba_absence[p];
            }

            for(int c = 0; c < room->nb_cameras(); c++) {
                pomsprintf(buffer, buffer_size, convergence_file_format, c, nb_frame, it);
                cout << "Saving " << buffer << "\n"; cout.flush();
                room->save_stochastic_view(buffer, c, result_proba_presence);
            }
        }

    }

    for(int p = 0; p < room->nb_positions(); p++) {
        (*result_proba_presence)[p] = 1 - proba_absence[p];
    }
}
