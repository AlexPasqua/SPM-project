#include <iostream>
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

    int n_attempts = 2;
    vector<long> clean_code_attempts(n_attempts), efficient_attempts(n_attempts);
    for (int t = 0; t < n_attempts; t++){
        cout << "Attempt " << t + 1 << " / " << n_attempts << endl;

        // read video
        VideoCapture cap(argv[1]);

        // take background image (i.e. frist frame)
        Mat background_rgb(5, 5, CV_8UC1);
        cap >> background_rgb;
        
        // convert background image to gray scale and smooth it
        Mat *gray_background = rgb2gray(&background_rgb, nullptr);
        Mat *smooth_gray_background = smooth(gray_background, nullptr);

        int rows = background_rgb.rows;
        int cols = background_rgb.cols;

        chrono::system_clock::time_point start, end;
        long elapsed_musecs;
        vector<long> clean_code, efficient_code;

        Mat *frame_gray = new Mat(rows, cols, CV_8UC1);
        Mat *frame = new Mat(rows, cols, CV_8UC1);
        while (true) {
            Mat frame_rgb;
            cap >> frame_rgb;
            if (frame_rgb.empty())
                break;
            
            // frame to grayscale
            frame_gray = rgb2gray(&frame_rgb, frame_gray);
            
            // smooth frame using 2 alternative functions and measuring performance
            {
                start = chrono::system_clock::now();
                frame = smooth_clean_code(frame_gray, frame);
                end = chrono::system_clock::now();
                elapsed_musecs = chrono::duration_cast<chrono::microseconds>(end - start).count();
                clean_code.push_back(elapsed_musecs);
            }
            {
                start = chrono::system_clock::now();
                frame = smooth(frame_gray, frame);
                end = chrono::system_clock::now();
                elapsed_musecs = chrono::duration_cast<chrono::microseconds>(end - start).count();
                efficient_code.push_back(elapsed_musecs);
            }
        }
        // free the memory
        delete gray_background, smooth_gray_background, frame_gray, frame;

        // compute average time for each method for the current attempt
        long sum_clean_code = 0, sum_efficient_code = 0;
        for (int i = 0; i < clean_code.size(); i++){
            sum_clean_code += clean_code[i];
            sum_efficient_code += efficient_code[i];
        }
        sum_clean_code /= clean_code.size();
        sum_efficient_code /= efficient_code.size();
        clean_code_attempts[t] = sum_clean_code;
        efficient_attempts[t] = sum_efficient_code;

        cout << "\tClean code: " << sum_clean_code << endl;
        cout << "\tEfficient code: " << sum_efficient_code << endl;
        cout << endl;
    }

    // compute average time for each method for all attempts
    long sum_clean_code_attempts = 0, sum_efficient_code_attempts = 0;
    for (int t = 0; t < n_attempts; t++){
        sum_clean_code_attempts += clean_code_attempts[t];
        sum_efficient_code_attempts += efficient_attempts[t];
    }
    sum_clean_code_attempts /= n_attempts;
    sum_efficient_code_attempts /= n_attempts;
    cout << "Average performance:" << endl;
    cout << "Clean code: " << sum_clean_code_attempts << endl;
    cout << "Efficient code: " << sum_efficient_code_attempts << endl;

    return 0;
}