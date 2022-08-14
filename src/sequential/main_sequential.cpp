#include "opencv2/opencv.hpp"

#include "auxiliary/utimer.hpp"
#include "sequential/sequential_funcs.hpp"


using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    // check number of CLI arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <video_path>" << endl;
        return -1;
    }

    // read video
    VideoCapture cap(argv[1]);

    // take background image (i.e. frist frame)
    Mat background_rgb(5, 5, CV_8UC1);
    cap >> background_rgb;
    
    // convert background image to gray scale and smooth it
    Mat *gray_background = rgb2gray(&background_rgb, nullptr);
    Mat *background = smooth(gray_background, nullptr);

    int rows = background_rgb.rows;
    int cols = background_rgb.cols;
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
        frame_gray = rgb2gray(&frame_rgb, frame_gray);

        // smooth the grayscale frame
        frame = smooth(frame_gray, frame);

        // motion detection
        if (motion_detect(background, frame, 10, 0.05)) {
            n_motion_frames++;
            cout << "Motion detected in frame " << n_frame << endl;
        }
        n_frame++;
    }

    // free the memory
    delete gray_background, background, frame_gray, frame;

    cout << "Number of frames with detected motion: " << n_motion_frames << endl;

    return n_motion_frames;
}