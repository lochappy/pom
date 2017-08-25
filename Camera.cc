#include "Camera.h"
#include "global.h"

Camera::Camera(int num_positions, ProbaView *proba_view)
{
    _proba_view = proba_view;
    _view_height = _proba_view->get_height();
    _view_width = _proba_view->get_width();
    vRects.resize(num_positions);
    mNeg.create(_view_height,_view_width,CV_64FC1);
    global_difference.set(global_mu_image_density, global_sigma_image_density);
}

void Camera::setRectangle(const int position, const Rectangle rect)
{
    ASSERT(position < vRects.size(), "Cammera::Rectangle::Index out of bounds");
    vRects[position].visible = rect.visible;
    vRects[position].xmax = rect.xmax;
    vRects[position].xmin = rect.xmin;
    vRects[position].ymax = rect.ymax;
    vRects[position].ymin = rect.ymin;

}

void Camera::compute_average_images(const cv::Mat &proba_absence)
{
    mNeg.setTo(1.0);
    const double *proba_absence_ptr = (const double*)proba_absence.data;
    for(size_t n = 0; n < vRects.size(); n++) if(proba_absence_ptr[n] <= global_proba_ignored) {
        Rectangle &r = vRects[n];
        if(r.visible)
            mNeg(cv::Rect(r.xmin, r.ymin, r.getWidth()+1,r.getHeight()+1)) *= proba_absence_ptr[n];
    }
}

void Camera::add_log_ratio(const cv::Mat &proba_absence, cv::Mat &sum)
{
    // Computes the average on the complete picture
    compute_average_images(proba_absence);

    const double s = ii_neg.compute_sum(mNeg);
    const double sv = ii_neg_view.compute_sum(mNeg, _proba_view->getCvMat());

    const scalar_t noise_proba = 0.01; // 1% of the scene can remain unexplained
    const scalar_t average_surface = _view_width * _view_height * (1 + noise_proba) - s;
    const scalar_t average_diff = average_surface + sv;

    // Cycles throw all positions and adds the log likelihood ratio to
    // the total sum for each

    for(int i = 0; i < vRects.size(); i++) {
        const Rectangle r = vRects[i];
        if(r.visible) {
            const scalar_t lambda = 1 - 1/proba_absence.at<double>(0,i);

            const scalar_t integral_neg =   ii_neg.integral(r.xmin, r.ymin, r.xmax + 1, r.ymax + 1);

            const scalar_t average_surface_givpre = average_surface +          integral_neg;
            const scalar_t average_surface_givabs = average_surface + lambda * integral_neg;

            const scalar_t integral_neg_view =  ii_neg_view.integral(r.xmin, r.ymin, r.xmax + 1, r.ymax + 1);

            const scalar_t average_diff_givpre = average_diff +           integral_neg - 2 * integral_neg_view;
            const scalar_t average_diff_givabs = average_diff + lambda * (integral_neg - 2 * integral_neg_view);

            const scalar_t log_mu0 = global_difference.log_proba(average_diff_givabs / average_surface_givabs);
            const scalar_t log_mu1 = global_difference.log_proba(average_diff_givpre / average_surface_givpre);

            sum.at<double>(0,i) += log_mu1 - log_mu0;

        }
    }
}
