#include <iostream>
#include "opencv2/opencv.hpp"

#include "parallel/parallel_funcs.hpp"
#include "shared_queue.cpp" //cpp because we need implementation of methods


void pick_and_comp(shared_queue<std::shared_ptr<cv::Mat>> *q) {
    std::shared_ptr<cv::Mat> frame_rgb = q->pop();
    std::cout << "rows:  " << frame_rgb->rows << std::endl;
}

void main_comp() {}