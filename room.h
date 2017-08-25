
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
// Modified by Matej Smid <smidm@cmp.felk.cvut.cz>								              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef ROOM_H
#define ROOM_H

#include "rectangle.h"
#include "proba_view.h"
#include "vector.h"
#include "opencv2/opencv.hpp"
#include "Camera.h"

using namespace std;

class Room {

  int _nb_cameras, _nb_positions;

  Vector<ProbaView *> _proba_views;

public:

  Room(int nb_cameras, int nb_positions, Vector<ProbaView *> proba_views);
  ~Room();
  std::vector< Camera > vCams;

  void setRectangle(const int camera_id, const int position, const Rectangle rect);

  inline int nb_positions() const { return _nb_positions; }
  inline int nb_cameras() const { return _nb_cameras; }
  inline int view_width(int n_camera) const { return _proba_views[n_camera]->get_width(); }
  inline int view_height(int n_camera) const { return _proba_views[n_camera]->get_height(); }
  inline ProbaView *get_view(int n_camera) const { return _proba_views[n_camera]; }
  inline cv::Mat get_view_CVMat(int n_camera) const {
//      cv::Mat room_view = _proba_views[n_camera]->getCvMat();
//      cv::imshow("room_view",room_view);
//      cv::waitKey();
      return _proba_views[n_camera]->getCvMat();
  }

  inline Rectangle avatar(int n_camera, int n_position) const {
    ASSERT(n_camera >= 0 && n_camera < _nb_cameras &&
           n_position >= 0 && n_position < _nb_positions,
           "Index out of bounds");
    return vCams[n_camera].vRects[n_position];
  }

  void save_stochastic_view(char *name, int ncam, const Vector<scalar_t> *proba_presence) const;
};

#endif
