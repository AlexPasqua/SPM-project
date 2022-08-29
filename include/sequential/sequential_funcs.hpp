#ifndef SEQFUNCS_HPP
#define SEQFUNCS_HPP

#include "opencv2/opencv.hpp"

// image conversion RGB to grayscale
cv::Mat * rgb2gray(cv::Mat *rgb_img, int nw);

// smoothing
void smooth_clean_code(cv::Mat *gray_img, cv::Mat *smooth_img, int nw);
void smooth(cv::Mat * gray_img, cv::Mat *smooth_img, int nw);

// motion detection
bool motion_detect(cv::Mat *img1, cv::Mat *img2, unsigned min_detect_diff,
                   float perc, int nw);

#endif