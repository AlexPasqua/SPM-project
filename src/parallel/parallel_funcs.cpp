#include <iostream>
#include <atomic>
#include "opencv2/opencv.hpp"

#include "parallel/parallel_funcs.hpp"
#include "sequential/sequential_funcs.hpp"
#include "shared_queue.cpp" //cpp because we need implementation of methods


void pick_and_comp(shared_queue<std::shared_ptr<cv::Mat>> *q,
                   bool *finished, cv::Mat *background,
                   int min_diff, float perc, std::atomic<int>& n_motion_frames) {
    int rows = background->rows;
    int cols = background->cols;
    cv::Mat frame_gray(rows, cols, CV_8UC1);
    cv::Mat frame(rows, cols, CV_8UC1);
    while (!(q->empty() && *finished)) {
        std::cout << q->empty() << " " << *finished << std::endl;
        std::shared_ptr<cv::Mat> frame_rgb = q->pop(finished);  // wait included in pop()
        if (frame_rgb == nullptr)
            continue;
        main_comp(background, frame_rgb.get(), &frame_gray, &frame, min_diff,
                  perc, n_motion_frames);
    }
    std::cout << "Thread finished" << std::endl;
}


void main_comp(cv::Mat *background, cv::Mat *frame_rgb, cv::Mat *frame_gray,
               cv::Mat *frame, int min_diff, float perc,
               std::atomic<int>& n_motion_frames) {
    rgb2gray(frame_rgb, frame_gray);
    smooth(frame_gray, frame);
    if (motion_detect(background, frame, min_diff, perc))
        n_motion_frames++;
}