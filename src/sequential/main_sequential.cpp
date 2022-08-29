#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>

#include "opencv2/opencv.hpp"

#include "auxiliary/timer.hpp"
#include "sequential/sequential_funcs.hpp"


using namespace std;
using namespace cv;

// prints the usage of the program in case the arguments are wrong
void print_usage(string prog_name) {
    cout << "Usage: " << prog_name << " <video_path> "
         << "[<n workers rgb2gray>] [<n workers smoothing] "
         << "[<n workers motion_detect>]" << endl;
    cout << "Arguments in square brackets are optional." << endl;
    cout << "Default values are 1 for each argument." << endl;
}

int main(int argc, char** argv) {
    // check CLI arguments
    if (argc < 2 || argc > 5) {
        print_usage(argv[0]);
        return -1;
    }
    int nw_rgb2gray = argc > 2 && atoi(argv[2]) > 0 ? atoi(argv[2]) : 1;
    int nw_smooth = argc > 3 && atoi(argv[3]) > 0 ? atoi(argv[3]) : 1;
    int nw_motion_detect = argc > 4 && atoi(argv[4]) > 0 ? atoi(argv[4]) : 1;

    // timer for the overall completion time
    timer<std::chrono::milliseconds> t("Overall completion time");

    // read video
    VideoCapture cap(argv[1]);

    // take background image (i.e. frist frame)
    int rows = cap.get(CAP_PROP_FRAME_HEIGHT);
    int cols = cap.get(CAP_PROP_FRAME_WIDTH);
    Mat *background_rgb = new Mat(rows, cols, CV_8UC3);
    cap >> *background_rgb;
    
    // convert background image to gray scale and smooth it
    Mat *background_gray = rgb2gray(background_rgb, nw_rgb2gray);
    Mat *background = new Mat(rows, cols, CV_8UC1);
    smooth(background_gray, background, nw_smooth);
    delete background_rgb, background_gray;

    Mat *frame_rgb = new Mat(rows, cols, CV_8UC3);
    Mat *frame_gray;
    Mat *frame = new Mat(rows, cols, CV_8UC1);
    bool motion_detected = false;
    int n_frames = 1, n_motion_frames = 0;
    chrono::system_clock::time_point start, stop;
    long cur_elapsed;
    std::vector<long> elapsed_times;
    
    // process all frames one by one
    while (true) {
        cap >> *frame_rgb;
        if (frame_rgb->empty())
            break;

        start = chrono::system_clock::now();
        
        // frame to grayscale
        frame_gray = rgb2gray(frame_rgb, nw_rgb2gray);

        // smooth the grayscale frame
        smooth(frame_gray, frame, nw_smooth);

        // motion detection
        if (motion_detect(background, frame, 10, 0.05, nw_motion_detect))
            n_motion_frames++;
        
        stop = chrono::system_clock::now();
        cur_elapsed = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
        elapsed_times.push_back(cur_elapsed);

        n_frames++;
    }

    // free the memory
    delete background, frame_rgb, frame_gray, frame;
    cap.release();

    // compute the average time to process a frame
    double avg_elapsed = std::reduce(
        elapsed_times.begin(), elapsed_times.end(), 0) /
        double(elapsed_times.size());

    cout << "Number of frames with detected motion: " << n_motion_frames << endl;
    cout << "Average time per frame: " << avg_elapsed << " ms" << endl;

    return n_motion_frames;
}