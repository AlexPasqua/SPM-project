#ifndef PARALLEL_FUNCS_HPP
#define PARALLEL_FUNCS_HPP

#include "opencv2/opencv.hpp"

// #include "../src/parallel/shared_queue.cpp"
#include "parallel/shared_queue.hpp"

void pick_and_comp(shared_queue<std::shared_ptr<cv::Mat>> *q);

void main_comp();

#endif