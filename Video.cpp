#include "Video.h"

Video::Video(int width, int height, int fps, string filename){
    video = VideoWriter(filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(width, height));
    this->width = width;
    this->height = height;
}

void Video::write(uint8_t* frame){
    // frame is in RGBA format. First we have to convert it to BGR

    Mat img(height, width, CV_8UC4, frame);
    Mat img_bgr;
    cvtColor(img, img_bgr, COLOR_RGBA2BGR);

    video.write(img_bgr);


}

void Video::release(){
    video.release();
}