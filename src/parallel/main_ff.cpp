#include <iostream>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <chrono>
#include <numeric>
#include "opencv2/opencv.hpp"

#include "sequential/sequential_funcs.hpp"
#include "parallel/parallel_funcs.hpp"
#include "auxiliary/timer.hpp"


using namespace std;
using namespace ff;

struct FrameWithMotionFlag {
    cv::Mat *frame;
    bool motion;
};

struct Emitter : ff_node_t<FrameWithMotionFlag> {
    cv::VideoCapture cap;
    int rows, cols;

    Emitter(cv::VideoCapture cap) : cap(cap) {
        rows = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        cols = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    }

    FrameWithMotionFlag *svc(FrameWithMotionFlag *input) {
        cv::Mat *frame = new cv::Mat(rows, cols, CV_8UC3);
        cap >> *frame;
        if (frame->empty()) {
            cap.release();
            cout << "Finished pushing frames" << endl;
            return EOS;
        }
        return new FrameWithMotionFlag{frame, false};
    }
};

struct Comp : ff_node_t<FrameWithMotionFlag> {
    cv::Mat *background, *frame_gray, *frame_smooth;
    int nw_rgb2gray, nw_smooth, nw_motion;
    unsigned int min_diff;
    float perc;

    Comp(cv::Mat *background, int nw_rgb2gray, int nw_smooth, int nw_motion,
         unsigned int min_diff, float perc) :
            background(background), nw_rgb2gray(nw_rgb2gray),
            nw_smooth(nw_smooth), nw_motion(nw_motion), min_diff(min_diff),
            perc(perc) {
                int rows = background->rows;
                int cols = background->cols;
                frame_smooth = new cv::Mat(rows, cols, CV_8UC1);
            }

    FrameWithMotionFlag *svc(FrameWithMotionFlag *frame_rgb) {
        frame_gray = rgb2gray(frame_rgb->frame, nw_rgb2gray);
        smooth(frame_gray, frame_smooth, nw_smooth);
        if (motion_detect(background, frame_smooth, min_diff, perc, nw_motion))
            frame_rgb->motion = true;
        return frame_rgb;
    }
};

struct Collector : ff_minode_t<FrameWithMotionFlag> {
    int n_motion_frames;

    Collector() : n_motion_frames(0) {}

    FrameWithMotionFlag *svc(FrameWithMotionFlag *frame_motion) {
        if (frame_motion->motion)
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
    int rows = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int cols = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    cv::Mat *background_rgb = new cv::Mat(rows, cols, CV_8UC3);
    cap >> *background_rgb;
    cv::Mat *background_gray = rgb2gray(background_rgb, nw_rgb2gray);
    cv::Mat *background = new cv::Mat(rows, cols, CV_8UC1);
    smooth(background_gray, background, nw_smooth);
    delete background_rgb, background_gray;

    // create workers
    std::vector<std::unique_ptr<ff_node>> workers;
    for (int i = 0; i < atoi(argv[2]); i++)
        workers.push_back(make_unique<Comp>(
            background, nw_rgb2gray, nw_smooth, nw_motion, 10, 0.05)
        );
    
    // create farm
    ff_Farm<FrameWithMotionFlag> farm(std::move(workers));
    Emitter emitter(cap);
    Collector collector;    // will contain the result
    farm.add_emitter(emitter);
    farm.add_collector(collector);

    // run
    farm.run_and_wait_end();

    // print results    
    cout << "Total number of motion frames: " << collector.n_motion_frames << endl;

    return 0;
}
