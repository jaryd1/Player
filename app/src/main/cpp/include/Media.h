//
// Created by Jarryd on 2019/9/27.
//

#ifndef APP_MEDIA_H
#define APP_MEDIA_H
#include "VideoDecoder.h"
#include "AudioDecoder.h"
#include "Displayer.h"
#include "OpenslHelp.h"
#include "MediaSyner.h"
#include "JavaCallBack.h"
#include "MediaCallBack.h"
#include <thread>
#include <chrono>

extern "C"{
#include <libavutil/imgutils.h>
#include "libswscale/swscale.h"
};
class Media:public MediaCallBack{
public:
    void initDisplayer(int width,int height) override ;
    void initAudio(AudioSpec* spec) override ;
    void displayPreviewFrame(AVFrame* frame) override ;
public:
    Media();
    MediaSyner* syner;
    Displayer* display;
    OpenslHelp* audio;

    char * url;
    void startThread();
    void onSurfaceCreated(ANativeWindow* window1);
    int fillAudioData(byte *data,int size);
    double fillVideoData(AVFrame**pAvFrame);
    void togglePlay();
    void initJavaCallBack(JavaVM* jvm,jobject obj){syner->initJavaCallBack(jvm,obj);}
    void seekPosition(float position){syner->seekPosision(position);}
    float getProgress(){return syner->getProgress();}
    float getBufferProgress(){return syner->getBufferProgress();}
    void updateDisplaySize(){ display->updateDisPlaySize();};
private:
    bool isPlaying = false;
    bool draw_first_frame = false;
    void init();
    void initSyner();
    static double VideoCallBack(AVFrame**pAvFrame,void* ctx);
    static int AudioCallBack(byte *data,int size,void*ctx);

};
#endif //APP_MEDIA_H
