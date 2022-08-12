#include <iostream>
#include "opencv2/opencv.hpp"

#include "utimer.cpp"


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


Mat smooth(Mat gray_img) {
    int rows = gray_img.rows;
    int cols = gray_img.cols;
    Mat smooth_img(rows, cols, CV_8UC1);

    int row_lower_offset, row_upper_offset, col_lower_offset, col_upper_offset;
    uchar sum;
    uint8_t n_neighbors;    // number of neighboring pixels (including central one)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i > 0 && i < rows - 1 && j > 0 && j < cols - 1) {
                smooth_img.at<uchar>(i, j) = (
                    gray_img.at<uchar>(i-1, j-1) +
                    gray_img.at<uchar>(i-1, j) +
                    gray_img.at<uchar>(i-1, j+1) +
                    gray_img.at<uchar>(i, j-1) +
                    gray_img.at<uchar>(i, j) +
                    gray_img.at<uchar>(i, j+1) +
                    gray_img.at<uchar>(i+1, j-1) +
                    gray_img.at<uchar>(i+1, j) +
                    gray_img.at<uchar>(i+1, j+1)
                ) / 9;
            }
            else {
                // find boundaries of the neighborhood
                row_lower_offset = i > 0 ? -1 : 0;
                row_upper_offset = i < rows - 1 ? 1 : 0;
                col_lower_offset = j > 0 ? -1 : 0;
                col_upper_offset = j < cols - 1 ? 1 : 0;
                n_neighbors = (row_upper_offset - row_lower_offset + 1) *
                            (col_upper_offset - col_lower_offset + 1);

                sum = 0;
                for (int r = row_lower_offset; r <= row_upper_offset; r++)
                    for (int c = col_lower_offset; c <= col_upper_offset; c++)
                        sum += gray_img.at<uchar>(i + r, j + c);
                
                smooth_img.at<uchar>(i, j) = sum / n_neighbors;
            }
        }
    }
    return smooth_img;
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
    Mat smooth_gray_background = smooth(gray_background);

    while (true) {
        Mat frame_rgb;
        cap >> frame_rgb;
        if (frame_rgb.empty())
            break;
        
        // frame to grayscale
        Mat frame_gray = rgb2gray(frame_rgb);

    }

    return 0;
}