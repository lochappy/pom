
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
//////////////////////////////////////////////////////////////////////////////////

#ifndef INTEGRAL_PROBA_VIEW_H
#define INTEGRAL_PROBA_VIEW_H

#include "proba_view.h"
#include "integral_array.h"
#include "opencv2/opencv.hpp"

class IntegralProbaView : public IntegralArray<scalar_t> {
public:
  IntegralProbaView(int view_width, int view_height) : IntegralArray<scalar_t>(view_width, view_height) {}

  cv::Mat getCvMat(){
      cv::Mat tmp(width,height,CV_64FC1,content);
      return tmp.t();
  }

  // Computes the integral image and returns the sum of all the
  // original image pixels

  inline scalar_t compute_sum(const ProbaView *m) {
    scalar_t *p = content, *pm = m->content;
    for(int x = 0; x < height; x++) *(p++) = 0;

    register scalar_t st = 0;
    register scalar_t sl;
    for(register int y = 1; y < width; y++) {
      sl = 0; *(p++) = sl;
      for(register int x = 0; x < height - 1; x++) {
        sl += *(pm++);
        *p = sl + *(p - height);
        p++;
      }
      st += sl;
    }

    return st;
  }

  // Computes the integral image and returns the sum of (2m-1)*b

  inline scalar_t compute_sum(const ProbaView *m, const ProbaView *b) {
    scalar_t *p = content, *pm = m->content, *pb = b->content;

    for(int x = 0; x < height; x++) *(p++) = 0;

    scalar_t st = 0;
    register scalar_t sl;
    for(int y = 1; y < width; y++) {
      sl = 0; *(p++) = 0;
      for(int x = 0; x < height - 1; x++) {
        st -= *pb;
        sl += *(pm++) * *(pb++);
        *p = sl + *(p - height);
        p++;
      }
      st += 2 * sl;
    }

    return st;
  }
};


class IntegralImage{
public:

//    IntegralImage(int iW,int iH){
//        mIntegral.create(iH+1,iW+1,CV_64FC1);
//    }
    IntegralImage(){}

    double compute_sum(cv::Mat &m){
        cv::Mat m_tmp;
        m.convertTo(m_tmp,CV_64FC1);
        cv::integral(m_tmp,mIntegral);
        return mIntegral.at<double>(mIntegral.rows-1,mIntegral.cols-1);
    }

    // Computes the integral image and returns the sum of (2m-1)*b
    double compute_sum(const cv::Mat &m, const cv::Mat &b){
        cv::Mat m_tmp, b_tmp, m_by_b;
        m.convertTo(m_tmp,CV_64FC1);
        b.convertTo(b_tmp,CV_64FC1);

        cv::multiply(m_tmp,b_tmp,m_by_b);
        cv::integral(m_by_b,mIntegral);

        cv::Scalar sum_b = cv::sum(b);

        return (2*mIntegral.at<double>(mIntegral.rows-1,mIntegral.cols-1)-sum_b.val[0]);
    }
    cv::Mat getIntegralImage(){return mIntegral;}


    // Integral on xmin <= x < xmax, ymin <= y < ymax
    // Thus, xmax and ymax can go up to m->width+1 and m->height+1 respectively

    inline double integral(int xmin, int ymin, int xmax, int ymax) const {
      ASSERT(xmin <= xmax && ymin <= ymax, "Inconsistent bounds for integral");
      ASSERT(xmin >= 0 && xmax < mIntegral.cols &&
             ymin >= 0 && ymax < mIntegral.rows, "Index out of bounds in IntegralImage()");
      return mIntegral.at<double>(ymax,xmax) + mIntegral.at<double>(ymin,xmin)
             -mIntegral.at<double>(ymin,xmax) -mIntegral.at<double>(ymax,xmin);
//      return Array<T>::heads[xmax][ymax] + Array<T>::heads[xmin][ymin]
//        - Array<T>::heads[xmax][ymin] - Array<T>::heads[xmin][ymax];
    }

private:
    cv::Mat mIntegral;
};

#endif
