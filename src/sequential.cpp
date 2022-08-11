#include <iostream>
#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;


Mat rgb2gray(Mat rgb_img) {
    int rows = rgb_img.rows;
    int cols = rgb_img.cols;
    Mat gray_img(rows, cols, CV_8UC1);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            gray_img.at<uchar>(i, j) = (
                rgb_img.at<Vec3b>(i, j)[0] + 
                rgb_img.at<Vec3b>(i, j)[1] +
                rgb_img.at<Vec3b>(i, j)[2]
            ) / 3;
        }
    }
    return gray_img;
}


int main(int argc, char** argv) {
    // check number of CLI arguments
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <video_path>" << endl;
        return -1;
    }

    // read video
    VideoCapture cap(argv[1]);

    // take background image (i.e. frist frame)
    Mat background_rgb(5, 5, CV_8UC1);
    cap >> background_rgb;
    int rows = background_rgb.rows;
    int cols = background_rgb.cols;

    Mat gray_background = rgb2gray(background_rgb);

    return 0;
}