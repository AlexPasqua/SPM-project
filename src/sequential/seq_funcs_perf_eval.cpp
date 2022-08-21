#include <iostream>
#include <numeric>
#include <cmath>
#include "opencv2/opencv.hpp"

#include "auxiliary/utimer.hpp"
#include "sequential/sequential_funcs.hpp"


using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    // check number of CLI arguments
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <video_path>"
                  << "<number of attempts>" << endl;
        return -1;
    }

    int n_attempts = std::atoi(argv[2]);
    vector<double> rgb2gray_attempts(n_attempts);
    vector<double> smooth_clean_attempts(n_attempts);
    vector<double> smooth_efficient_attempts(n_attempts);
    vector<double> motion_detect_attempts(n_attempts);
    for (int t = 0; t < n_attempts; t++){
        cout << "Attempt " << t + 1 << " / " << n_attempts << endl;

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

        // variables for performance evaluation
        chrono::system_clock::time_point start, end;
        long elapsed_musecs;
        vector<long> t_rgb2gray, t_smooth_clean_code, t_smooth_efficient, t_motion_detect;

        // allocate matrices for storing the results
        Mat *frame_gray = new Mat(rows, cols, CV_8UC1);
        Mat *frame = new Mat(rows, cols, CV_8UC1);

        // loop over the video
        int n_motion_frames = 0;
        while (true) {
            Mat frame_rgb;
            cap >> frame_rgb;
            if (frame_rgb.empty())
                break;
            
            // measure latency of conversion from RGB to grayscale
            start = chrono::system_clock::now();
            rgb2gray(&frame_rgb, frame_gray);
            end = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(end - start).count();
            t_rgb2gray.push_back(elapsed_musecs);
            
            // measure latency of smoothing with function with cleaner code
            start = chrono::system_clock::now();
            smooth_clean_code(frame_gray, frame);
            end = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(end - start).count();
            t_smooth_clean_code.push_back(elapsed_musecs);

            // measure latency of smoothing with function with more efficient code
            start = chrono::system_clock::now();
            smooth(frame_gray, frame);
            end = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(end - start).count();
            t_smooth_efficient.push_back(elapsed_musecs);

            // measure latency of motion detection
            start = chrono::system_clock::now();
            if (motion_detect(background, frame, 10, 0.05)) {n_motion_frames++;}
            end = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(end - start).count();
            t_motion_detect.push_back(elapsed_musecs);
        }
        // free the memory
        delete gray_background, background, frame_gray, frame;

        // compute average time for each stage for the current attempt
        double size = double(t_rgb2gray.size());    // all vectors have the same size
        
        double cur_avg_rgb2gray = std::accumulate(
            t_rgb2gray.begin(), t_rgb2gray.end(), 0) / size;
        
        double cur_avg_smooth_clean_code = std::accumulate(
            t_smooth_clean_code.begin(), t_smooth_clean_code.end(), 0) / size;
        
        double cur_avg_smooth_efficient = std::accumulate(
            t_smooth_efficient.begin(), t_smooth_efficient.end(), 0) / size;
        
        double cur_avg_motion_detect = std::accumulate(
            t_motion_detect.begin(), t_motion_detect.end(), 0) / size;
        
        rgb2gray_attempts[t] = cur_avg_rgb2gray;
        smooth_clean_attempts[t] = cur_avg_smooth_clean_code;
        smooth_efficient_attempts[t] = cur_avg_smooth_efficient;
        motion_detect_attempts[t] = cur_avg_motion_detect;

        // print results for current attempt
        cout << "\tRGB to grayscale: " << cur_avg_rgb2gray << " microseconds" << endl;
        cout << "\tSmoothing (clean code): " << cur_avg_smooth_clean_code << " microseconds" << endl;
        cout << "\tSmoothing (efficient code): " << cur_avg_smooth_efficient << " microseconds" << endl;
        cout << "\tMotion detection: " << cur_avg_motion_detect << " microseconds" << endl;
        cout << endl;
    }

    // compute average latency for each method for all attempts
    double size = double(n_attempts);
    
    double avg_rgb2gray = std::accumulate(
        rgb2gray_attempts.begin(), rgb2gray_attempts.end(), 0) / size;
    
    double avg_smooth_clean_code = std::accumulate(
        smooth_clean_attempts.begin(), smooth_clean_attempts.end(), 0) / size;
    
    double avg_smooth_efficient = std::accumulate(
        smooth_efficient_attempts.begin(), smooth_efficient_attempts.end(), 0) / size;
    
    double avg_motion_detect = std::accumulate(
        motion_detect_attempts.begin(), motion_detect_attempts.end(), 0) / size;
    
    // print results
    cout << "Average performance:" << endl;
    cout << "RGB to grayscale:\t\t" << avg_rgb2gray << " microseconds" << endl;
    cout << "Smoothing (clean code):\t\t" << avg_smooth_clean_code << " microseconds" << endl;
    cout << "Smoothing (efficient code):\t" << avg_smooth_efficient << " microseconds"<< endl;
    cout << "Motion detection:\t\t" << avg_motion_detect << " microseconds" << endl;

    return 0;
}