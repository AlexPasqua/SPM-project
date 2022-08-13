#include "opencv2/opencv.hpp"
#include "sequential/rgb2gray.hpp"


using namespace cv;

/**
 * @brief takes a RGB image (cv::Mat with 3 channels) and returns a grayscale
 * image (cv::Mat with 1 channel)
 * 
 * @param rgb_img cv::Mat with 3 channels (R-G-B)
 * @param gray_img (optional) cv::Mat with 1 channel (grayscale) where to put the result
 * @return grayscale version of 'rgb_img' (a cv::Mat with 1 channel)
 */
Mat * rgb2gray(Mat *rgb_img, Mat *gray_img = nullptr) {
    int rows = rgb_img->rows;
    int cols = rgb_img->cols;
    if (!gray_img)
        gray_img = new Mat(rows, cols, CV_8UC1);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            gray_img->at<uchar>(i, j) = (
                rgb_img->at<Vec3b>(i, j)[0] + 
                rgb_img->at<Vec3b>(i, j)[1] +
                rgb_img->at<Vec3b>(i, j)[2]
            ) / 3;
        }
    }
    return gray_img;
}