#include <iostream>
#include <chrono>

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
    Mat background_rgb;
    cap >> background_rgb;
    
    int rows = background_rgb.rows;
    int cols = background_rgb.cols;
    
    // convert background image to gray scale and smooth it
    Mat *gray_background = new Mat(rows, cols, CV_8UC1);
    Mat *background = new Mat(rows, cols, CV_8UC1);
    rgb2gray(&background_rgb, gray_background, nw_rgb2gray);
    smooth(gray_background, background, nw_smooth);

    Mat frame_rgb;
    Mat *frame_gray = new Mat(rows, cols, CV_8UC1);
    Mat *frame = new Mat(rows, cols, CV_8UC1);
    bool motion_detected = false;
    int n_frame = 1, n_motion_frames = 0;
    
    // process all frames one by one
    while (true) {
        cap >> frame_rgb;
        if (frame_rgb.empty())
            break;
        
        // frame to grayscale
        rgb2gray(&frame_rgb, frame_gray, nw_rgb2gray);

        // smooth the grayscale frame
        smooth(frame_gray, frame, nw_smooth);

        // motion detection
        if (motion_detect(background, frame, 10, 0.05, nw_motion_detect)) {
            n_motion_frames++;
            // cout << "Motion detected in frame " << n_frame << endl;
        }
        n_frame++;
    }

    // free the memory
    delete gray_background, background, frame_gray, frame;
    cap.release();

    cout << "Number of frames with detected motion: " << n_motion_frames << endl;

    return n_motion_frames;
}