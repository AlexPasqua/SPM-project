#ifndef PARALLEL_FUNCS_HPP
#define PARALLEL_FUNCS_HPP

#include <queue>
#include <atomic>
#include "opencv2/opencv.hpp"

#include "parallel/shared_queue.hpp"


void pick_and_comp(shared_queue<cv::Mat> *q, const int th_num,
                   cv::Mat *background, int nw_rgb2gray, int nw_smooth,
                   int nw_motion_detect, int min_diff, float perc,
                   std::atomic<int>& n_motion_frames);

void main_comp(cv::Mat *background, cv::Mat *frame_rgb, cv::Mat *frame_gray,
               cv::Mat *frame, int nw_rgb2gray, int nw_smooth,
               int nw_motion_detect, int min_diff, float perc,
               std::atomic<int>& n_motion_frames);

void print_usage_parallel_prog(const std::string prog_name);

#endif