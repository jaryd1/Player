//
// Created by Jarryd on 2019/10/25.
//

#ifndef PLAYER_MEDIASYNERCALLBACK_H
#define PLAYER_MEDIASYNERCALLBACK_H
class MediaSynerCallBack{
public:
    virtual double getFrameTime()=0;
    virtual double getClockDiffByVideoClock()=0;
    virtual void updateVideoClock(double pts){};
    virtual void updateFrameTime(double frameTime){};
    virtual void showLoading(){};
    virtual void hideLoading(){};
    virtual int processAudio(AudioFrame* audioFrame){};
    virtual void updateVideoSize(int width,int height){};
};
#endif //PLAYER_MEDIASYNERCALLBACK_H
