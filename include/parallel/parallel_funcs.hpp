#ifndef PARALLEL_FUNCS_HPP
#define PARALLEL_FUNCS_HPP

#include <queue>
#include <atomic>
#include "opencv2/opencv.hpp"

#include "parallel/shared_queue.hpp"


void pick_and_comp(shared_queue<cv::Mat> *q, const int th_num,
                   cv::Mat *background, int nw_rgb2gray, int nw_smooth,
                   int nw_motion_detect, int min_diff, float perc,
                   std::atomic<int>& n_motion_frames, double &avg_frame_latency,
                    double &avg_ts,
                    int &first, int &n_ts);

void main_comp(cv::Mat *background, cv::Mat *frame_rgb, int nw_rgb2gray,
               int nw_smooth, int nw_motion_detect, int min_diff, float perc,
               std::atomic<int>& n_motion_frames,
               std::chrono::system_clock::time_point &ts_start,
               std::chrono::system_clock::time_point &ts_stop,
               double &avg_ts,
               int &first, int &n_ts);

void print_usage_parallel_prog(const std::string prog_name);

#endif