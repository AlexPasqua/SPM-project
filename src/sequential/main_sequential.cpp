#include <iostream>
#include "opencv2/opencv.hpp"

#include "auxiliary/timer.hpp"
#include "sequential/sequential_funcs.hpp"


using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    // check number of CLI arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <video_path>" << endl;
        return -1;
    }

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
    rgb2gray(&background_rgb, gray_background);
    smooth(gray_background, background);

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
        rgb2gray(&frame_rgb, frame_gray);

        // smooth the grayscale frame
        smooth(frame_gray, frame);

        // motion detection
        if (motion_detect(background, frame, 10, 0.05)) {
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