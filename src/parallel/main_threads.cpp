#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <numeric>
#include <queue>
#include "opencv2/opencv.hpp"

#include "parallel/parallel_funcs.hpp"
#include "sequential/sequential_funcs.hpp"
#include "parallel/shared_queue.hpp"
#include "auxiliary/timer.hpp"


using namespace std;

int main(int argc, char** argv) {
    if (argc < 3 || argc > 6) {
        print_usage_parallel_prog(argv[0]);
        return -1;
    }
    int nw_rgb2gray = argc > 3 && atoi(argv[3]) > 0 ? atoi(argv[3]) : 1;
    int nw_smooth = argc > 4 && atoi(argv[4]) > 0 ? atoi(argv[4]) : 1;
    int nw_motion_detect = argc > 5 && atoi(argv[5]) > 0 ? atoi(argv[5]) : 1;

    // timer for the overall completion time
    timer<std::chrono::milliseconds> tc("Overall completion time");

    // read video
    cv::VideoCapture cap(argv[1]);
    int rows = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int cols = cap.get(cv::CAP_PROP_FRAME_WIDTH);

    // take and process background image (i.e. frist frame)
    cv::Mat *background_rgb = new cv::Mat(rows, cols, CV_8UC3);
    cap >> *background_rgb;
    cv::Mat *background_gray = rgb2gray(background_rgb, nw_rgb2gray);
    cv::Mat *background = new cv::Mat(rows, cols, CV_8UC1);
    smooth(background_gray, background, nw_smooth);
    delete background_rgb, background_gray;

    // shared queue for frames
    shared_queue<cv::Mat> q;

    // atomic variable to store the result
    std::atomic<int> n_motion_frames(0);

    // start threads
    int n_threads = atoi(argv[2]);
    std::vector<std::thread> threads;
    std::vector<double> frames_latencies(n_threads);
    // std::vector<std::chrono::system_clock::time_point> ts_starts(n_threads);
    // std::vector<std::chrono::system_clock::time_point> ts_stops(n_threads);
    std::vector<double> avg_ts(n_threads);
    std::vector<int> first(n_threads, 1);
    std::vector<int> n_ts(n_threads, 0);
    for (int i = 0; i < n_threads; i++)
        threads.push_back(std::thread(pick_and_comp, &q, i, background,
                                      nw_rgb2gray, nw_smooth, nw_motion_detect,
                                      10, 0.05, std::ref(n_motion_frames),
                                      std::ref(frames_latencies[i]),
                                    //   std::ref(ts_starts[i]), std::ref(ts_stops[i]),
                                      std::ref(avg_ts[i]),
                                      std::ref(first[i]),
                                      std::ref(n_ts[i])));     

    // put frames in the queue for elaboration
    while (true) {
        cv::Mat *frame_rgb = new cv::Mat(rows, cols, CV_8UC3);
        cap >> *frame_rgb;
        if (frame_rgb->empty())
            break;
        q.push(frame_rgb);
    }
    q.no_more_pushes();
    cout << "Finished pushing frames" << endl;

    // join threads
    for (auto& t : threads)
        if (t.joinable())
            t.join();
    
    // print number of motion frames
    cout << "Number of frames with detected motion: " << n_motion_frames << endl;

    // print average frame latency
    double avg_frame_latency = std::accumulate(
        frames_latencies.begin(), frames_latencies.end(), 0.0) /
        double(frames_latencies.size());
    cout << "Average latency per frame: " << avg_frame_latency << " us" << endl;

    for (int i = 0; i < avg_ts.size(); i++)
        avg_ts[i] /= double(n_ts[i]);
    double avg_service_time = std::accumulate(
        avg_ts.begin(), avg_ts.end(), 0.0) /
        double(avg_ts.size());
    cout << "Average service time: " << avg_service_time << " ms" << endl;

    return 0;
}