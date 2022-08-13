#include "opencv2/opencv.hpp"

#include "sequential/rgb2gray.hpp"
#include "sequential/smoothing.hpp"


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
    Mat *smooth_gray_background = smooth_efficient(gray_background, nullptr);

    int rows = background_rgb.rows;
    int cols = background_rgb.cols;

    Mat *frame_gray = new Mat(rows, cols, CV_8UC1);
    Mat *frame = new Mat(rows, cols, CV_8UC1);
    while (true) {
        Mat frame_rgb;
        cap >> frame_rgb;
        if (frame_rgb.empty())
            break;
        
        // frame to grayscale
        frame_gray = rgb2gray(&frame_rgb, frame_gray);

        // smooth the grayscale frame
        frame = smooth_efficient(frame_gray, frame);
    }

    // free the memory
    delete gray_background, smooth_gray_background, frame_gray, frame;

    return 0;
}