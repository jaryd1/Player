//
// Created by Jarryd on 2019/9/27.
//
#include "VideoDecoder.h"




void VideoDecoder::clearBuffer() {
    std::unique_lock<std::mutex> lk(frame_mutex);
    Frames.clear();
    lk.unlock();
}

double VideoDecoder::getFrameDuration() {
    if(frame_rate.num && frame_rate.den){
        return av_q2d((AVRational){frame_rate.den,frame_rate.num});
    }else{
        return 0;
    }
}