#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include "sequential/sequential_funcs.hpp"


using namespace std;
using namespace cv;

/**
 * @brief takes a RGB image (cv::Mat with 3 channels) and converts it into
 * a grayscale image.
 * 
 * The grayscale 2D image is put in place of channel 0 of the 3D rgb image.
 * The pointer to that channel 0 is returned.
 * 
 * @param rgb_img cv::Mat with 3 channels (R-G-B)
 * @param nw number of threads to use (if 1, sequential version)
 * @return pointer to a grayscale version of 'rgb_img' (a cv::Mat with 1 channel)
 */
cv::Mat * rgb2gray(Mat *rgb_img, int nw) {
    cv::Mat *gray_img = &(rgb_img[0]);  // points to channel 0 of rgb_img
    int rows = gray_img->rows;
    int cols = gray_img->cols;
    #pragma omp parallel for num_threads(nw)
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            // note: gray_image->at(i,j) points to the same pixel as rgb_img->at(i,j)[0]
            gray_img->at<uchar>(i, j) = (rgb_img->at<Vec3b>(i, j)[0] + 
                                         rgb_img->at<Vec3b>(i, j)[1] +
                                         rgb_img->at<Vec3b>(i, j)[2]) / 3;
    return gray_img;
}


/**
 * @brief smooths a grayscale image.
 * Version of the function with the cleanest code
 * 
 * @param gray_img: grayscale image to be smoothed
 * @param smooth_img: Mat where to put the result
 * @param nw: number of threads to use (if 1, sequential version)
 * @return the smoothed image
 */
void smooth_clean_code(Mat *gray_img, Mat *smooth_img, int nw) {
    int rows = gray_img->rows;
    int cols = gray_img->cols;
    int row_lower_offset, row_upper_offset, col_lower_offset, col_upper_offset;
    uchar sum;
    uint8_t n_neighbors;    // number of neighboring pixels (including central one)
    #pragma omp parallel for num_threads(nw)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
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
                    sum += gray_img->at<uchar>(i + r, j + c);
            
            smooth_img->at<uchar>(i, j) = sum / n_neighbors;
        }
    }
}


/**
 * @brief smooths a grayscale image.
 * A more efficient version of the function, since it removes 2 nested cycles
 * for the great majority of the pixels.
 * 
 * @param gray_img: grayscale image to be smoothed
 * @param smooth_img: Mat where to put the result
 * @param nw number of workers to use (if 1, sequential version)
 * @return the smoothed image
 */
void smooth(Mat *gray_img, Mat *smooth_img, int nw) {
    int rows = gray_img->rows;
    int cols = gray_img->cols;
    int row_lower_offset, row_upper_offset, col_lower_offset, col_upper_offset;
    uchar sum;
    uint8_t n_neighbors;    // number of neighboring pixels (including central one)

    // for each pixel NOT on the border of the image
    #pragma omp parallel for num_threads(nw)
    for (int i = 1; i < rows-1; i++) {
        for (int j = 1; j < cols-1; j++) {
            smooth_img->at<uchar>(i, j) = (
                gray_img->at<uchar>(i-1, j-1) +
                gray_img->at<uchar>(i-1, j) +
                gray_img->at<uchar>(i-1, j+1) +
                gray_img->at<uchar>(i, j-1) +
                gray_img->at<uchar>(i, j) +
                gray_img->at<uchar>(i, j+1) +
                gray_img->at<uchar>(i+1, j-1) +
                gray_img->at<uchar>(i+1, j) +
                gray_img->at<uchar>(i+1, j+1)
            ) / 9;
        }
    }

    vector<int> row_borders_idxs = {0, rows - 1};
    vector<int> col_borders_idxs = {0, cols - 1};
    // for each pixel on the border of the image
    for (auto &i : row_borders_idxs) {
        // compute row offsets
        row_lower_offset = i > 0 ? -1 : 0;
        row_upper_offset = i < rows - 1 ? 1 : 0;
        for (auto &j : col_borders_idxs) {
            // compute column offsets and number of neighboring pixels
            col_lower_offset = j > 0 ? -1 : 0;
            col_upper_offset = j < cols - 1 ? 1 : 0;
            n_neighbors = (row_upper_offset - row_lower_offset + 1) *
                          (col_upper_offset - col_lower_offset + 1);
            
            sum = 0;
            for (int r = row_lower_offset; r <= row_upper_offset; r++)
                for (int c = col_lower_offset; c <= col_upper_offset; c++)
                    sum += gray_img->at<uchar>(i + r, j + c);
            
            smooth_img->at<uchar>(i, j) = sum / n_neighbors;
        }
    }
}


/**
 * @brief checks whether 2 grayscale images differ for more than a certain
 * percentage of the pixels.
 * 
 * @param img1: grayscale image
 * @param img2: another grayscale image
 * @param min_detect_diff: minimum absolute difference between 2 pixels to be counted as different
 * @param perc: percentage of different pixels to consider the images as differing from each other
 * @param nw number of workers to use (if 1, sequential version)
 * @return true if the images differ for more than 'perc'% of their pixels, false otherwise
 */
bool motion_detect(Mat *img1, Mat *img2, unsigned int min_detect_diff,
                   float perc, int nw) {
    int rows = img1->rows;
    int cols = img1->cols;

    // count number of pixels that differ from the corresponding one in the other image
    // for more than a certain amount (min_detect_diff) (because of smoothing)
    unsigned n_different_pixels = 0;
    #pragma omp parallel for reduction(+:n_different_pixels) num_threads(nw)
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (abs(img1->at<uchar>(i, j) - img2->at<uchar>(i, j)) > min_detect_diff)
                n_different_pixels++;
    
    
    // if more than a certain percentage of the pixels are different, then there is motion
    float perc_different_pixels = float(n_different_pixels) / float(rows * cols);
    
    if (perc_different_pixels > perc)
        return true;
    return false;
}
