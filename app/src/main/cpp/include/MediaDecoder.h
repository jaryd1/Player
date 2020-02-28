//
// Created by Jarryd on 2019/9/30.
//

#ifndef PLAYER_MEDIADECODER_H
#define PLAYER_MEDIADECODER_H
#include <list>
#include <mutex>
#include <condition_variable>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};
class MediaDecoder{
public:
    MediaDecoder(){};
    virtual ~MediaDecoder(){};
    std::list<AVFrame*>Frames;
    std::mutex frame_mutex;
    std::condition_variable fd;
    AVCodecContext* cc_ctx = nullptr;
    AVCodec* cc = nullptr;
    AVStream* stream = nullptr;
    AVRational frame_rate;
    double time_base;
    float buff_pts = 0;
    volatile int  read_eof;
    void putFrame(AVFrame* frame);
    AVFrame* getFrame();
    virtual void clearBuffer(){};
    virtual double getFrameDuration(){return 0;}

};

#endif //PLAYER_MEDIADECODER_H
