#include <iostream>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <chrono>
#include "opencv2/opencv.hpp"

#include "sequential/sequential_funcs.hpp"
#include "parallel/parallel_funcs.hpp"
#include "auxiliary/timer.hpp"


using namespace std;
using namespace ff;

std::atomic<int> n_motion_frames(0);

struct Emitter : ff_node_t<cv::Mat> {
    cv::VideoCapture cap;
    cv::Mat *frame;

    Emitter(cv::VideoCapture cap) : cap(cap) {}

    cv::Mat *svc(cv::Mat *input) {
        while (true) {
            frame = new cv::Mat();
            cap >> *frame;
            if (frame->empty()) {
                cap.release();
                cout << "Finished pushing frames" << endl;
                return EOS;
            }
            return frame;
        }
    }
};

struct Comp : ff_node_t<cv::Mat> {
    int nw_rgb2gray, nw_smooth, nw_motion, rows, cols;
    unsigned int min_diff;
    float perc;

    Comp(int nw_rgb2gray, int nw_smooth, int nw_motion, unsigned int min_diff, float perc) :
        nw_rgb2gray(nw_rgb2gray), nw_smooth(nw_smooth), nw_motion(nw_motion), min_diff(min_diff), perc(perc) {}

    cv::Mat *svc(cv::Mat *frame_rgb) {
        rows = frame_rgb->rows;
        cols = frame_rgb->cols;
        cv::Mat frame_gray(rows, cols, CV_8UC1);
        cv::Mat frame_smooth(rows, cols, CV_8UC1);
        rgb2gray(frame_rgb, &frame_gray, nw_rgb2gray);
        smooth(&frame_gray, &frame_smooth, nw_smooth);
        if (motion_detect(&frame_smooth, &frame_gray, min_diff, perc, nw_motion))
            n_motion_frames++;
        return GO_ON;
    }
};


int main(int argc, char **argv) {
    if (argc < 3 || argc > 6) {
        print_usage_parallel_prog(argv[0]);
        return -1;
    }
    int nw_rgb2gray = argc > 3 && atoi(argv[3]) > 0 ? atoi(argv[3]) : 1;
    int nw_smooth = argc > 4 && atoi(argv[4]) > 0 ? atoi(argv[4]) : 1;
    int nw_motion = argc > 5 && atoi(argv[5]) > 0 ? atoi(argv[5]) : 1;

    // timer for the overall completion time
    timer<chrono::milliseconds> tc("Overall completion time");

    // process background
    cv::VideoCapture cap(argv[1]);
    cv::Mat background_rgb;
    cap >> background_rgb;
    int rows = background_rgb.rows;
    int cols = background_rgb.cols;
    cv::Mat background_gray(rows, cols, CV_8UC1);
    cv::Mat background(rows, cols, CV_8UC1);
    rgb2gray(&background_rgb, &background_gray, nw_rgb2gray);
    smooth(&background_gray, &background, nw_smooth);

    // create workers
    std::vector<unique_ptr<ff_node>> workers;
    for (int i = 0; i < atoi(argv[2]); i++)
        workers.push_back(make_unique<Comp>(nw_rgb2gray, nw_smooth, nw_motion,
                                            10, 0.05));
    
    // create farm
    ff_Farm<cv::Mat> farm(std::move(workers));
    Emitter emitter(cap);
    farm.add_emitter(emitter);
    farm.remove_collector();

    // run
    farm.run_and_wait_end();

    // print results
    cout << "Number of motion frames: " << n_motion_frames << endl;

    return 0;
}
