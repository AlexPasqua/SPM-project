#include <iostream>
#include <atomic>
#include <queue>
#include "opencv2/opencv.hpp"

#include "parallel/parallel_funcs.hpp"
#include "sequential/sequential_funcs.hpp"
#include "parallel/shared_queue.hpp"


/**
 * @brief base function executed by threads. Pops frames and runs the
 * "main_comp" on them.
 * 
 * @param q pointer to the shared queue
 * @param th_num number of the current thread (for logging purposes)
 * @param background the background image to be passed to "main_comp"
 * @param min_diff minimum difference between 2 pixels to be considered different
 * @param perc percentage of different pixels to consider a frame different from background
 * @param n_motion_frames variable where to save the number of motion frames
 */
void pick_and_comp(shared_queue<std::shared_ptr<cv::Mat>> *q, const int th_num,
                   cv::Mat *background, int nw_rgb2gray, int nw_smooth,
                   int nw_motion_detect, int min_diff, float perc,
                   std::atomic<int>& n_motion_frames) {
    // create matrices to store intermediate computations on the frame
    int rows = background->rows;
    int cols = background->cols;
    cv::Mat frame_gray(rows, cols, CV_8UC1);
    cv::Mat frame(rows, cols, CV_8UC1);
    
    // continue looping until the queue is empty and the video is finished
    while (!(q->empty() && q->get_finished())) {
        // pop frame from the queue (synchronization included in pop())
        std::shared_ptr<cv::Mat> frame_rgb = q->pop();

        // when the video is finished and the queue is empty, threads waiting
        // to pop a frame will return nullptr
        if (frame_rgb == nullptr)
            break;
        
        // run the main comp on the frame just popped
        main_comp(background, frame_rgb.get(), &frame_gray, &frame, nw_rgb2gray,
                  nw_smooth, nw_motion_detect, min_diff, perc, n_motion_frames);
    }
    std::cout << "Thread " << th_num << " finished" << std::endl;
}


/**
 * @brief main composition of sequential stages to run on frames.
 * 
 * For each frame, it turns it into grayscale, runs smoothing and check
 * if some motion is detected in the frame w.r.t. the background.
 * 
 * @param q pointer to the shared queue
 * @param background the background image
 * @param min_diff minimum difference between 2 pixels to be considered different
 * @param perc percentage of different pixels to consider a frame different from background
 * @param n_motion_frames variable where to save the number of motion frames
 */
void main_comp(cv::Mat *background, cv::Mat *frame_rgb, cv::Mat *frame_gray,
               cv::Mat *frame, int nw_rgb2gray, int nw_smooth,
               int nw_motion_detect, int min_diff, float perc,
               std::atomic<int>& n_motion_frames) {
    // convert frame to grayscale
    rgb2gray(frame_rgb, frame_gray, nw_rgb2gray);
    // smooth frame
    smooth(frame_gray, frame, nw_smooth);
    // check if motion is detected
    if (motion_detect(background, frame, min_diff, perc, nw_motion_detect))
        n_motion_frames++;
}