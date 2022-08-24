#include <iostream>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
// #include <chrono>
#include "opencv2/opencv.hpp"

#include "sequential/sequential_funcs.hpp"
#include "auxiliary/timer.hpp"


using namespace std;
using namespace ff;

std::atomic<int> n_motion_frames(0);

struct Emitter : ff_node_t<cv::Mat> {
    cv::VideoCapture cap;

    Emitter(cv::VideoCapture cap) : cap(cap) {}

    cv::Mat *svc(cv::Mat *input) {
        while (true) {
            cv::Mat *frame = new cv::Mat;
            cap >> *frame;
            if (frame->empty()) {
                cap.release();
                return EOS;
            }
            return frame;
        }
    }
};

struct Comp : ff_node_t<cv::Mat> {
    cv::Mat *svc(cv::Mat *frame_rgb) {
        int rows = frame_rgb->rows;
        int cols = frame_rgb->cols;
        cv::Mat frame_gray(rows, cols, CV_8UC1);
        cv::Mat frame_smooth(rows, cols, CV_8UC1);
        rgb2gray(frame_rgb, &frame_gray, 1);
        smooth(&frame_gray, &frame_smooth, 1);
        if (motion_detect(&frame_smooth, &frame_gray, 0, 0.0, 1))
            n_motion_frames++;
        return GO_ON;
    }
};


int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <video_path> <number of workers>" << endl;
        return -1;
    }

    // timer for the overall completion time
    timer<chrono::milliseconds> tc("Overall completion time");

    // process background
    cv::VideoCapture cap(argv[1]);
    cv::Mat background_rgb;
    int rows = background_rgb.rows;
    int cols = background_rgb.cols;
    cv::Mat background_gray(rows, cols, CV_8UC1);
    cv::Mat background(rows, cols, CV_8UC1);
    cap >> background_rgb;
    rgb2gray(&background_rgb, &background_gray, 1);
    smooth(&background_gray, &background, 1);

    // create workers
    std::vector<unique_ptr<ff_node>> workers;
    for (int i = 0; i < atoi(argv[2]); i++)
        workers.push_back(make_unique<Comp>());
    
    // create farm
    ff_Farm<cv::Mat> farm(std::move(workers));
    Emitter emitter(cap);
    farm.add_emitter(emitter);
    farm.remove_collector();

    // run
    farm.run_and_wait_end();

    return 0;
}
