//
// Created by Jarryd on 2019/10/25.
//

#ifndef PLAYER_MEDIASYNERCALLBACK_H
#define PLAYER_MEDIASYNERCALLBACK_H
class MediaSynerCallBack{
public:
    virtual double getFrameTime()=0;
    virtual double getClockDiffByVideoClock()=0;
    virtual void updateVideoClock(double pts)=0;
    virtual void updateFrameTime(double frameTime)=0;
    virtual void showLoading()=0;
    virtual void hideLoading()=0;
    virtual int processAudio(AudioFrame* audioFrame)=0;
    virtual void updateVideoSize(int width,int height)=0;
    virtual void processKeyFrame(AVFrame* frame)=0;
};
#endif //PLAYER_MEDIASYNERCALLBACK_H
