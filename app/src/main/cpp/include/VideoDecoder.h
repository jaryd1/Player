//
// Created by Jarryd on 2019/9/27.
//

#ifndef APP_DECODER_H
#define APP_DECODER_H
#include "MediaDecoder.h"



class VideoDecoder:public MediaDecoder{
public:
    VideoDecoder():MediaDecoder(){};
    ~VideoDecoder(){};
    void clearBuffer() override ;
    double getFrameDuration() override ;
};
#endif //APP_DECODER_H
