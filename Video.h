#include <opencv2/opencv.hpp>
#include <iostream>
 
using namespace std;
using namespace cv;

class Video {
    public:
        Video(int width, int height, int fps, string filename);
        void write(uint8_t* frame);
        void release();

        uint width;
        uint height;

    private:
        VideoWriter video;
};