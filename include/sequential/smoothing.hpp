#ifndef SMOOTHING_HPP
#define SMOOTHING_HPP

#include "opencv2/opencv.hpp"

cv::Mat * smooth_clean_code(cv::Mat *gray_img, cv::Mat *smooth_img);
cv::Mat * smooth_efficient(cv::Mat * gray_img, cv::Mat *smooth_img);

#endif