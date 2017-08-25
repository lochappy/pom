#ifndef INTEGRALIMAGE_H
#define INTEGRALIMAGE_H
#include "misc.h"
#include "opencv2/opencv.hpp"

class IntegralImage
{
public:
    IntegralImage();
    double compute_sum(cv::Mat &m);

    cv::Mat getIntegralImage(){return mIntegral;}

    // Computes the integral image and returns the sum of (2m-1)*b
    double compute_sum(const cv::Mat &m, const cv::Mat &b);

    // Integral on xmin <= x < xmax, ymin <= y < ymax
    // Thus, xmax and ymax can go up to m->width+1 and m->height+1 respectively
    inline double integral(int xmin, int ymin, int xmax, int ymax) const {
      ASSERT(xmin <= xmax && ymin <= ymax, "Inconsistent bounds for integral");
      ASSERT(xmin >= 0 && xmax < mIntegral.cols &&
             ymin >= 0 && ymax < mIntegral.rows, "Index out of bounds in IntegralImage()");
      return mIntegral.at<double>(ymax,xmax) + mIntegral.at<double>(ymin,xmin)
             -mIntegral.at<double>(ymin,xmax) -mIntegral.at<double>(ymax,xmin);
    }


private:
    cv::Mat mIntegral;
};

#endif // INTEGRALIMAGE_H
