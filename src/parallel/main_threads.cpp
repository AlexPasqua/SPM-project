#include <iostream>
#include <thread>
#include "opencv2/opencv.hpp"

#include "parallel/parallel_funcs.hpp"
#include "../parallel/shared_queue.cpp"
#include "../auxiliary/timer.cpp"


using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <video_path>" << " <number_of_threads>" << endl;
        return -1;
    }

    // timer for the overall completion time
    timer<std::chrono::milliseconds> tc("Overall completion time");

    // read video
    VideoCapture cap(argv[1]);

    // take background image (i.e. frist frame)
    Mat background_rgb;
    cap >> background_rgb;

    // start threads
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < atoi(argv[2]); i++)
        threads.push_back(std::thread(main_comp, 4));

    // put frames in the queue for elaboration
    shared_queue<std::shared_ptr<Mat>> q;
    Mat frame_rgb;
    while (true) {
        cap >> frame_rgb;
        if (frame_rgb.empty())
            break;
        q.push(std::make_shared<Mat>(frame_rgb));
    }

    // join threads
    for (auto& t : threads)
        if (t.joinable())
            t.join();

    return 0;
}