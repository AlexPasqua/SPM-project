#include <iostream>
#include <numeric>
#include <chrono>
#include "opencv2/opencv.hpp"

#include "auxiliary/timer.hpp"
#include "sequential/sequential_funcs.hpp"


using namespace std;
using namespace cv;

// prints the usage of the program in case the arguments are wrong
void print_usage(string prog_name) {
    cout << "Usage: " << prog_name << " <video_path> " << "<number of attempts >"
         << "[<n workers rgb2gray>] [<n workers smoothing] "
         << "[<n workers motion_detect>]" << endl;
    cout << "Arguments in square brackets are optional." << endl;
    cout << "Default values are 1 for each argument." << endl;
}

int main(int argc, char** argv) {
    // check number of CLI arguments
    if (argc < 3 || argc > 6) {
        print_usage(argv[0]);
        return -1;
    }
    int nw_rgb2gray = argc > 3 && atoi(argv[3]) > 0 ? atoi(argv[3]) : 1;
    int nw_smooth = argc > 4 && atoi(argv[4]) > 0 ? atoi(argv[4]) : 1;
    int nw_motion_detect = argc > 5 && atoi(argv[5]) > 0 ? atoi(argv[5]) : 1;

    int n_attempts = std::atoi(argv[2]);
    vector<double> rgb2gray_attempts(n_attempts);
    vector<double> smooth_clean_attempts(n_attempts);
    vector<double> smooth_efficient_attempts(n_attempts);
    vector<double> motion_detect_attempts(n_attempts);
    for (int t = 0; t < n_attempts; t++){
        cout << "Attempt " << t + 1 << " / " << n_attempts << endl;

        // read video
        VideoCapture cap(argv[1]);
        int rows = cap.get(CAP_PROP_FRAME_HEIGHT);
        int cols = cap.get(CAP_PROP_FRAME_WIDTH);

        // take background image (i.e. frist frame)
        Mat *background_rgb = new Mat(rows, cols, CV_8UC3);
        cap >> *background_rgb;

        // convert background image to gray scale and smooth it
        Mat *gray_background = rgb2gray(background_rgb, nw_rgb2gray);
        Mat *background = new Mat(rows, cols, CV_8UC1);
        smooth(gray_background, background, nw_smooth);
        delete background_rgb, gray_background;

        // variables for performance evaluation
        chrono::system_clock::time_point start, stop;
        long elapsed_musecs;
        vector<long> t_rgb2gray, t_smooth_clean_code, t_smooth_efficient, t_motion_detect;

        // allocate matrices for storing the results
        Mat *frame_rgb = new Mat(rows, cols, CV_8UC3);
        Mat *frame_gray;
        Mat *frame = new Mat(rows, cols, CV_8UC1);

        // loop over the video
        int n_motion_frames = 0;
        while (true) {
            cap >> *frame_rgb;
            if (frame_rgb->empty())
                break;

            // measure latency of conversion from RGB to grayscale
            start = chrono::system_clock::now();
            frame_gray = rgb2gray(frame_rgb, nw_rgb2gray);
            stop = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(stop - start).count();
            t_rgb2gray.push_back(elapsed_musecs);
            
            // measure latency of smoothing with function with cleaner code
            start = chrono::system_clock::now();
            smooth_clean_code(frame_gray, frame);
            stop = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(stop - start).count();
            t_smooth_clean_code.push_back(elapsed_musecs);

            // measure latency of smoothing with function with more efficient code
            start = chrono::system_clock::now();
            smooth(frame_gray, frame, nw_smooth);
            stop = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(stop - start).count();
            t_smooth_efficient.push_back(elapsed_musecs);

            // measure latency of motion detection
            start = chrono::system_clock::now();
            if (motion_detect(background, frame, 10, 0.05, nw_motion_detect))
                n_motion_frames++;
            stop = chrono::system_clock::now();
            elapsed_musecs = chrono::duration_cast<chrono::microseconds>(stop - start).count();
            t_motion_detect.push_back(elapsed_musecs);
        }
        // free the memory
        delete background, frame_rgb, frame_gray, frame;

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
        cout << "\tSum of the 3 stages (w/ efficient smoothing): "
             << cur_avg_rgb2gray + cur_avg_smooth_efficient  + cur_avg_motion_detect
             << " microseconds" << endl;
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
    cout << "Sum of the 3 stages (w/ efficient smoothing): "
         << avg_rgb2gray + avg_smooth_efficient  + avg_motion_detect
         << " microseconds" << endl;

    return 0;
}