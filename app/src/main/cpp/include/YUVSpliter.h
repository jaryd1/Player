//
// Created by Jarryd on 2019/9/28.
//

#ifndef APP_YUVSPLITER_H
#define APP_YUVSPLITER_H


extern "C"{
#include "libavutil/frame.h"
};
class YUVSpliter{
public:
     unsigned char* y;
     unsigned char* u;
     unsigned char* v;
    int lastwidth,lastheight;
    void spliter(AVFrame* frame);
    void spliterYUV420(AVFrame* frame);
    void spliterNV12(AVFrame* frame);

};

#endif //APP_YUVSPLITER_H
