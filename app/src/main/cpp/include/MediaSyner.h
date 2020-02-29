//
// Created by Jarryd on 2019/10/8.
//

#ifndef PLAYER_MEDIASYNER_H
#define PLAYER_MEDIASYNER_H

#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "AllDecoder.h"
#include "JavaCallBack.h"
#include "MediaClock.h"
#include "MediaSynerCallBack.h"
#include "MediaCallBack.h"
#include "sonic.h"
#include "Sonicwrapper.h"

extern "C"{
#include "libavutil/time.h"
};

class MediaSyner:public MediaSynerCallBack{
private:
    Sonicwrapper *sonicwrapper;
    MediaCallBack* mediaCallBack;
    AllDecoder * decoder;
    JavaCallBack* javaCallBack;
    std::mutex mMuxter;
    std::condition_variable mCond;
    void decode();
    void notify();
    void callbackTimeUpdate();

    volatile  bool paused = false;
    volatile  bool has_preview_frame = false;
    MediaClock* audioClock;
    MediaClock* videoClock;
private:
    double frame_time = 0;
    bool refreshFrameTime = 1;
    volatile bool isLoading = false;
    bool isPaused(){return paused;}
public:
    double getFrameTime() override ;
    double getClockDiffByVideoClock() override ;
    void updateVideoClock(double pts) override ;
    void updateFrameTime(double frameTime) override ;
    void showLoading();
    void hideLoading();
    int processAudio(AudioFrame* audioFrame);
    void processKeyFrame(AVFrame* frame) override;
    void updateVideoSize(int width,int height);
public:
    void seekPosision(float position);
    void initJavaCallBack(JavaVM* jvm,jobject obj){
        javaCallBack->init(jvm,obj);
    }
    MediaSyner(MediaCallBack* mediaCallBack);
    void init(char* url);
    int fillAudioData(byte *data,int size,int displayInited);
    double fillVideoData(AVFrame** frame);
    float getProgress(){return (float)decoder->getCurrentPts()/1000000;}
    float getBufferProgress(){return decoder->getBufferProgress();}
    void togglePlaying(bool statue);
    void setSpeed(float speed);
};

#endif //PLAYER_MEDIASYNER_H
