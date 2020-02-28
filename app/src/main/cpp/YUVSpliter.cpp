//
// Created by Jarryd on 2019/9/28.
//
#include "YUVSpliter.h"
#include "androidlog.h"

void YUVSpliter::spliterYUV420(AVFrame *frame) {
    if(lastwidth != frame->width || lastheight !=frame->height){
        y = new unsigned char[frame->width*frame->height];
        u = new unsigned char[frame->width*frame->height/4];
        v = new unsigned char[frame->width*frame->height/4];
        lastwidth = frame->width;
        lastheight = frame->height;
    }
    for (int i = 0; i < frame->height; ++i) {
        memcpy(y+i*frame->width,frame->data[0]+i*frame->linesize[0],
                frame->width* sizeof(unsigned char));
    }
    for (int i = 0; i < frame->height / 2; ++i) {
        memcpy(u+i*frame->width/2,frame->data[1]+i*frame->linesize[1],
               sizeof(unsigned char)* frame->width/2);
        memcpy(v+i*frame->width/2,frame->data[2]+i*frame->linesize[2],
               sizeof(unsigned char)* frame->width/2);
    }
}

void YUVSpliter::spliterNV12(AVFrame *frame) {
    if(lastwidth != frame->width || lastheight !=frame->height){
        y = (unsigned char*)av_malloc(frame->width*frame->height);
        u = (unsigned char*)av_malloc(frame->width*frame->height/2);
        lastwidth = frame->width;
        lastheight = frame->height;
    }
    for (int i = 0; i < frame->height; ++i) {
        memcpy(y+i*frame->width,frame->data[0]+i*frame->linesize[0],
               frame->width* sizeof(unsigned char));
    }
    for (int i = 0; i < frame->height / 2; ++i) {
        memcpy(u+i*frame->width,frame->data[1]+i*frame->linesize[1],
               sizeof(unsigned char)* frame->width);
    }
}

void YUVSpliter::spliter(AVFrame *frame) {
    switch (frame->format){
        case AV_PIX_FMT_YUV420P:
            spliterYUV420(frame);
            break;
        case AV_PIX_FMT_NV12:
        case AV_PIX_FMT_NV21:
            spliterNV12(frame);
            break;
    }
}

