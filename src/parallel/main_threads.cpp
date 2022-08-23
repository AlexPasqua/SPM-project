#include <iostream>
#include <thread>
#include <atomic>
#include "opencv2/opencv.hpp"

#include "parallel/parallel_funcs.hpp"
#include "sequential/sequential_funcs.hpp"
#include "parallel/shared_queue.hpp"
#include "auxiliary/timer.hpp"


using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <video_path>" << " <number_of_threads>" << endl;
        return -1;
    }

    // timer for the overall completion time
    timer<std::chrono::milliseconds> tc("Overall completion time");

    // read video
    cv::VideoCapture cap(argv[1]);

    // take and process background image (i.e. frist frame)
    cv::Mat background_rgb;
    cap >> background_rgb;
    int rows = background_rgb.rows;
    int cols = background_rgb.cols;
    cv::Mat background_gray(rows, cols, CV_8UC1);
    cv::Mat background(rows, cols, CV_8UC1);
    rgb2gray(&background_rgb, &background_gray);
    smooth(&background_gray, &background);

    // shared queue for frames
    shared_queue<std::shared_ptr<cv::Mat>> q;

    // atomic variable to store the result
    std::atomic<int> n_motion_frames(0);

    // start threads
    std::vector<std::thread> threads;
    for (int i = 0; i < atoi(argv[2]); i++)
        threads.push_back(std::thread(pick_and_comp, &q, i, &background, 0, 0.0,
                                      std::ref(n_motion_frames)));

    // put frames in the queue for elaboration
    cv::Mat frame_rgb;
    while (true) {
        cap >> frame_rgb;
        if (frame_rgb.empty())
            break;
        q.push(std::make_shared<cv::Mat>(frame_rgb));
    }
    q.no_more_pushes();
    cout << "Finished pushing frames" << endl;

    // join threads
    for (auto& t : threads)
        if (t.joinable())
            t.join();
    
    // print number of motion frames
    cout << "Number of frames with detected motion: " << n_motion_frames << endl;
    return 0;
}