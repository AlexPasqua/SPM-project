#ifndef SEQFUNCS_HPP
#define SEQFUNCS_HPP

#include "opencv2/opencv.hpp"

// image conversion RGB to grayscale
cv::Mat * rgb2gray(cv::Mat *rgb_img, cv::Mat *gray_img);

// smoothing
cv::Mat * smooth_clean_code(cv::Mat *gray_img, cv::Mat *smooth_img);
cv::Mat * smooth(cv::Mat * gray_img, cv::Mat *smooth_img);

// motion detection
bool motion_detect(cv::Mat *img1, cv::Mat *img2, unsigned min_detect_diff, float perc);

#endif