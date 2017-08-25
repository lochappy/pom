#include "IntegralImage.h"

IntegralImage::IntegralImage()
{

}

double IntegralImage::compute_sum(cv::Mat &m){
    cv::Mat m_tmp;
    m.convertTo(m_tmp,CV_64FC1);
    cv::integral(m_tmp,mIntegral);
    return mIntegral.at<double>(mIntegral.rows-1,mIntegral.cols-1);
}

// Computes the integral image and returns the sum of (2m-1)*b
double IntegralImage::compute_sum(const cv::Mat &m, const cv::Mat &b){
    cv::Mat m_tmp, b_tmp, m_by_b;
    m.convertTo(m_tmp,CV_64FC1);
    b.convertTo(b_tmp,CV_64FC1);

    cv::multiply(m_tmp,b_tmp,m_by_b);
    cv::integral(m_by_b,mIntegral);

    cv::Scalar sum_b = cv::sum(b);

    return (2*mIntegral.at<double>(mIntegral.rows-1,mIntegral.cols-1)-sum_b.val[0]);
}
