
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
// Modified by Matej Smid <smidm@cmp.felk.cvut.cz>						              		//
//////////////////////////////////////////////////////////////////////////////////

#ifndef POM_SOLVER_H
#define POM_SOLVER_H

#include "misc.h"
#include "normal_law.h"
#include "room.h"
#include "vector.h"
#include "opencv2/opencv.hpp"
#include "IntegralImage.h"

class POMSolver {

  // At each pixel the proba for the pixel to be off
  std::vector< cv::Mat> vNegs;

  // At each pixel, 0 if the view is 0, and the proba for the pixel to
  // be off if the view is 1 (or, more mathematically: neg * view)

  //Vector<ProbaView *> neg_view;

  // Integral images to speed-up computation
  std::vector< IntegralImage> vIINegs;
  std::vector< IntegralImage> vIINegView;

  // Distribution of surface_difference / surface_synthetic

  NormalLaw global_difference;

  void compute_average_images(const int camera,
                              const Room *room,
                              const cv::Mat &proba_absence);

  // Adds to every sum[i] the value log(P(X_i = 1 | V_camera) / P(X_i
  // = 0 | V_camera)), given the other P(X_j = 1 | V)

  void add_log_ratio(const int camera,
                     const Room *room,
                     const cv::Mat &proba_absence,
                     cv::Mat &sum);

public:

  POMSolver(Room *room);

  // Uses the computation above for the various cameras and the prior
  // to refresh proba_absence. Iterates as many times as
  // specified. The two last parameters are used only to save images
  // showing the convergence

  void solve(Room *room,
             Vector<scalar_t> *prior,
             Vector<scalar_t> *result_proba_presence,
             int nb_frame,
             char *convergence_file_format);
};

#endif
