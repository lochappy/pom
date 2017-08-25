#ifndef CAMERA_H
#define CAMERA_H
#include "opencv2/opencv.hpp"
#include "rectangle.h"
#include "misc.h"
#include "IntegralImage.h"
#include "normal_law.h"
#include "proba_view.h"

class Camera
{
public:
    Camera(int num_positions, ProbaView * proba_view);
    void setRectangle(const int position, const Rectangle rect);
    std::vector< Rectangle > vRects;

    void compute_average_images(const cv::Mat &proba_absence);
    void add_log_ratio(const cv::Mat &proba_absence, cv::Mat &sum);

private:
    ProbaView * _proba_view;

    // At each pixel the proba for the pixel to be off
    cv::Mat mNeg;

    int _view_height, _view_width;

    // Integral images to speed-up computation
    IntegralImage ii_neg, ii_neg_view;

    // Distribution of surface_difference / surface_synthetic
    NormalLaw global_difference;
};

#endif // CAMERA_H
