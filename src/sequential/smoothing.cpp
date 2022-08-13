#include "opencv2/opencv.hpp"
#include "sequential/smoothing.hpp"


using namespace std;
using namespace cv;

/**
 * @brief smooths a grayscale image.
 * Version of the function with the cleanest code
 * 
 * @param gray_img: grayscale image to be smoothed
 * @param smooth_img: (optional) Mat where to put the result
 * @return the smoothed image
 */
Mat * smooth_clean_code(Mat *gray_img, Mat *smooth_img = nullptr) {
    int rows = gray_img->rows;
    int cols = gray_img->cols;
    if (!smooth_img)
        smooth_img = new Mat(rows, cols, CV_8UC1);  // matrix where to save the result

    int row_lower_offset, row_upper_offset, col_lower_offset, col_upper_offset;
    uchar sum;
    uint8_t n_neighbors;    // number of neighboring pixels (including central one)
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
    return smooth_img;
}


/**
 * @brief smooths a grayscale image.
 * A more efficient version of the function, since it removes 2 nested cycles
 * for the great majority of the pixels.
 * 
 * @param gray_img: grayscale image to be smoothed
 * @param smooth_img: (optional) Mat where to put the result
 * @return the smoothed image
 */
Mat * smooth_efficient(Mat *gray_img, Mat *smooth_img = nullptr) {
    int rows = gray_img->rows;
    int cols = gray_img->cols;
    if (!smooth_img)
        smooth_img = new Mat(rows, cols, CV_8UC1);  // matrix where to save the result

    int row_lower_offset, row_upper_offset, col_lower_offset, col_upper_offset;
    uchar sum;
    uint8_t n_neighbors;    // number of neighboring pixels (including central one)

    // for each pixel NOT on the border of the image
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
    return smooth_img;
}