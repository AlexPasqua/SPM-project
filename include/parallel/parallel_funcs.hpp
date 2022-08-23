#ifndef PARALLEL_FUNCS_HPP
#define PARALLEL_FUNCS_HPP

#include "opencv2/opencv.hpp"

#include "parallel/shared_queue.hpp"


void pick_and_comp(shared_queue<std::shared_ptr<cv::Mat>> *q, const int th_num,
                   cv::Mat *background, int min_diff, float perc,
                   std::atomic<int>& n_motion_frames);

void main_comp(cv::Mat *background, cv::Mat *frame_rgb, cv::Mat *frame_gray,
               cv::Mat *frame, int min_diff, float perc,
               std::atomic<int>& n_motion_frames);

#endif