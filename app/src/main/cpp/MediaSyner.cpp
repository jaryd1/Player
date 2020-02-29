//
// Created by Jarryd on 2019/10/8.
//
#include <thread>
#include "MediaSyner.h"
#include "androidlog.h"
#include "Tool.h"



MediaSyner::MediaSyner(MediaCallBack* mediaCallBack) {
    this->mediaCallBack = mediaCallBack;
    decoder = new AllDecoder(this);
    javaCallBack = new JavaCallBack();
    audioClock = new MediaClock();
    videoClock = new MediaClock();
    sonicwrapper = new Sonicwrapper();
}

void MediaSyner::init(char*url) {
    LOGE("url %s",url);
    decoder->init(url);


    if(decoder->isValidVideo()){
        javaCallBack->setVideoSize(decoder->getVideoWidth(),decoder->getVideoHeight());
        mediaCallBack->initDisplayer(decoder->getVideoWidth(),decoder->getVideoHeight());
    }
    if(decoder->isValidAudio()){
        mediaCallBack->initAudio(decoder->getAudioSpec());
        sonicwrapper->init(decoder->getAudioSpec()->sample_rate,decoder->getAudioSpec()->channels);
        setSpeed(1.0f);
    }

    javaCallBack->callDuration(decoder->getDuration());
    std::thread decode(&MediaSyner::decode,this);
    decode.detach();

}


double MediaSyner::getFrameTime() {
    if(refreshFrameTime){
        frame_time = av_gettime_relative()/1000000;
        refreshFrameTime = false;
    }
    return frame_time;
}

double MediaSyner::getClockDiffByVideoClock() {
    return videoClock->getClock() - audioClock->getClock();
}

void MediaSyner::updateVideoClock(double pts) {
    videoClock->setClock(pts);
}

void MediaSyner::updateFrameTime(double frameTime) {
    frame_time = frameTime;
}

void MediaSyner::showLoading() {
    if(!isLoading){
        isLoading = true;
        javaCallBack->showLoading();
    }
}

void MediaSyner::hideLoading() {
    if(isLoading){
        isLoading = false;
        refreshFrameTime = true;
        javaCallBack->hideLoading();
    }
}

int MediaSyner::processAudio(AudioFrame* audioFrame) {
    byte *output = NULL;
    int len = sonicwrapper->processAudio(audioFrame->getData(),audioFrame->getNbSamples(),audioFrame->getSize(),
            &output);
    audioFrame->setData(output);
    return len;
}

void MediaSyner::processKeyFrame(AVFrame *frame) {
    mediaCallBack->displayPreviewFrame(frame);
}

void MediaSyner::updateVideoSize(int width, int height) {
    javaCallBack->setVideoSize(width,height);
}


void MediaSyner::togglePlaying(bool play) {
    refreshFrameTime = true;
    paused = !play;
}

void MediaSyner::decode() {
    LOGE("start decode");
    while(true) {
        std::unique_lock<std::mutex> lck(mMuxter);
        if(has_preview_frame)
            mCond.wait(lck);
        else
            has_preview_frame = true;
        lck.unlock();
        if(!decoder->isEof()) {
            decoder->decode();
        }else{
            break;
        }
    }
    LOGE("decode finish");
}

void MediaSyner::setSpeed(float speed) {
    sonicwrapper->setSpeed(speed);
    decoder->setSpeed(speed);
}

void MediaSyner::seekPosision(float position) {
    if(decoder){
        LOGE("pos %.5f",position);
        decoder->seekPosition(position);
    }
    refreshFrameTime = 1;
}

void MediaSyner::notify() {
    if(decoder != NULL && decoder->needDecode()) {
        mCond.notify_all();
    }
}

void MediaSyner::callbackTimeUpdate() {
    int64_t start_time = decoder->getFmtCtx()->start_time;
    if(start_time >0 && start_time != AV_NOPTS_VALUE){
        start_time =av_rescale(start_time,1000,AV_TIME_BASE);
    }
    double  clock = decoder->isValidAudio()?audioClock->getClock():videoClock->getClock();
    int64_t pos =0;
    if(isnan(clock)){
        pos = 0;
    }else{
        pos = (int64_t)(clock*1000);
    }
    if(pos <0 || pos < start_time){
        pos = 0;
    }
    javaCallBack->callCurrentPos((long) pos,decoder->getBufferProgress(),
                                 decoder->getDuration());
}


double MediaSyner::fillVideoData(AVFrame **frame) {
    notify();
    double sleep_time;
    if(!isLoading){
        sleep_time =  decoder->fillVideoData(frame);
    }else{
        sleep_time = 0.02;
        *frame = NULL;
    }
    if(!decoder->isValidAudio()){
        callbackTimeUpdate();
    }
    return sleep_time;
}

int MediaSyner::fillAudioData(byte *data, int size,int displayInited) {
    notify();
    if(decoder->isCompleted()){
        return -1;
    }
    int len =  decoder->fillAudioData(data,size,displayInited);
    if(len == 0){
        len = size;
    }
    audioClock->setClock(decoder->getCurrentPts() - len / (double)(decoder->getAudioDecoder()->getAudioSpec()->getBytePS()),
                         av_gettime_relative() / 1000000.0);

    callbackTimeUpdate();

    return len;
}


