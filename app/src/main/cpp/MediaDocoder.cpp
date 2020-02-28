//
// Created by Jarryd on 2019/9/30.
//
#include "MediaDecoder.h"
#include "androidlog.h"


void MediaDecoder::putFrame(AVFrame *frame) {
    std::unique_lock<std::mutex> lk(frame_mutex);
    Frames.push_back(frame);
    lk.unlock();
    fd.notify_all();
}


AVFrame* MediaDecoder::getFrame() {
    AVFrame* frame;
    std::unique_lock<std::mutex> lk(frame_mutex);
    fd.wait(lk,[this](){return Frames.size() >0;});
    frame = Frames.front();
    Frames.pop_front();
    lk.unlock();
    fd.notify_all();
    return frame;
}
