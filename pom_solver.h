
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
