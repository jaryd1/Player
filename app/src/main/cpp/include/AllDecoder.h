//
// Created by Jarryd on 2019/10/9.
//

#ifndef PLAYER_ALLDECODER_H
#define PLAYER_ALLDECODER_H

#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "MediaSynerCallBack.h"

#define NON_STREAM -1
#define MIN_THRESHOULD 0.04
#define MAX_THRESHOULD 0.1
#define MAX_FRAME_DURATION 3600

class AllDecoder{
private:
    MediaSynerCallBack* synerCallBack;
    AudioDecoder* audioDecoder;
    VideoDecoder* videoDecoder;
    AVFormatContext* fmt_ctx;
    int video_index = NON_STREAM;
    int audio_index = NON_STREAM;
    AudioFrame* currentAudio = NULL;
    int currentAudioPos = 0;
    double  currentPts = 0;
    double last_video_pts = 0;
    volatile bool  eof = 0;
    volatile bool  completed = false;
    volatile bool seek_req = false;
    volatile bool update_seek_after_pts =false;
    int fps = -1;
    float seek_target ;
    //second
    double minbufferduration = 3;
    double maxbufferduration = 10;
    double bufferduration = 0;
    double  speed = 1;
public:
    void setSpeed(double speed);

private:
    volatile bool buffered = true;
private:
    void initDecoder(MediaDecoder* decoder,int stream_index);
    int handleVideo(AVPacket*avPacket);
    int handleAudio(AVPacket*packet);
    void flushVideo();
    void flushAudio();
    void seek();
    int getVidepFps();
    double caculateDelay(double lastDuration);
    double caculatDuration(double newPts);
    void checkBufferEnough();
public:
    AllDecoder(MediaSynerCallBack* mediaSynerCallBack);
    void init(char*url);
    void decode();
    int fillAudioData(unsigned char*data,int size,int displayInited);
    double fillVideoData(AVFrame** frame);
    int getAudioBufferSize(){return audioDecoder->getPCMSize();}
    int getVideoBufferSize(){return videoDecoder->Frames.size();}
    VideoDecoder* getVideoDecoder(){return videoDecoder;}
    AudioDecoder* getAudioDecoder(){return audioDecoder;}
    float getDuration();
    void seekPosition(float position);
    double getCurrentPts() const;
    AVFormatContext* getFmtCtx(){return fmt_ctx;}
    bool isEof(){return eof;}

    float getBufferProgress(){return audioDecoder->buff_pts;}
    bool isValidAudio(){return audio_index != NON_STREAM;}
    bool isValidVideo(){return video_index != NON_STREAM;}
    bool isSeeking(){return seek_req;}
    bool isCompleted(){return completed;}
    bool needDecode(){return bufferduration < maxbufferduration;}
    AudioSpec* getAudioSpec(){return audioDecoder->getAudioSpec();}
    int getVideoWidth();
    int getVideoHeight();
};
#endif //PLAYER_ALLDECODER_H
