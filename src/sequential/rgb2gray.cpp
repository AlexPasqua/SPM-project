#include "opencv2/opencv.hpp"


using namespace cv;

/**
 * @brief takes a RGB image (cv::Mat with 3 channels) and returns a grayscale
 * image (cv::Mat with 1 channel)
 * 
 * @param rgb_img cv::Mat with 3 channels (R-G-B)
 * @return grayscale version of 'rgb_img' (a cv::Mat with 1 channel)
 */
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