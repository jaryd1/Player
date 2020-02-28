//
// Created by Jarryd on 2019/9/27.
//
#include "Media.h"


Media::Media() {
    syner = new MediaSyner(this);
    display = new Displayer();
    audio = new OpenslHelp();
}


void Media::init() {
    initSyner();
    isPlaying = true;
}

void Media::initDisplayer(int width,int height) {
    display->open(width,height,VideoCallBack,this);
}

void Media::initAudio(AudioSpec* spec) {
    audio->open(spec,AudioCallBack,this);
}

void Media::initSyner() {
    syner->init(url);
}

void Media::togglePlay() {
    isPlaying = !isPlaying;
    syner->togglePlaying(isPlaying);

    if(audio){
        audio->togglePlayState(isPlaying);
    }
}

int Media::fillAudioData(byte *data, int size) {
    int len = syner->fillAudioData(data,size,display->isInited());
    return len;
}

int Media::AudioCallBack(byte *data, int size,void*ctx) {
    Media* media = (Media*)ctx;
    return media->fillAudioData(data,size);

}

double Media::fillVideoData(AVFrame **pAvFrame) {
    return syner->fillVideoData(pAvFrame);
}

double Media::VideoCallBack(AVFrame **pAvFrame,void*ctx) {
    Media* media = (Media*)ctx;
    return media->fillVideoData(pAvFrame);
}

void Media::onSurfaceCreated(ANativeWindow *window1) {
    display->SurfaceCreated(window1);
}


void Media::startThread() {
    std::thread init(&Media::init,this);
    init.detach();
}
