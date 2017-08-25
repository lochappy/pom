
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
{}

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
            room->vCams[c].add_log_ratio(m_proba_absence, m_sum);

        cv::Mat m_exp, m_err;
        cv::exp(m_log_prior_ratio+m_sum ,m_exp);
        cv::Mat m_np = global_smoothing_coefficient*m_proba_absence + (1-global_smoothing_coefficient)/( 1 + m_exp);
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
    cv::Mat mRoom(37,36,CV_64FC1,m_result_proba_presence.data);
    cv::imshow("mRoom",mRoom);
    cv::waitKey();
}
