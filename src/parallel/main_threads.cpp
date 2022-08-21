#include <iostream>
#include <thread>
#include "opencv2/opencv.hpp"

#include "parallel/parallel_funcs.hpp"
#include "../auxiliary/utimer.cpp"


using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <video_path>" << "<number_of_threads>" << endl;
        return -1;
    }

    // utimer to print the overall completion time at the end of the program
    utimer<std::chrono::milliseconds> tc("Overall completion time");

    // read video
    VideoCapture cap(argv[1]);

    // take background image (i.e. frist frame)
    Mat background_rgb(5, 5, CV_8UC1);
    cap >> background_rgb;

    // start threads
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < atoi(argv[2]); i++) {
        threads.push_back(std::thread(main_comp, 4));
    }

    // join threads
    for (auto& t : threads)
        t.join();

    return 0;
}