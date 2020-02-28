//
// Created by Jarryd on 2019/9/29.
//

#ifndef PLAYER_DISPLAYER_H
#define PLAYER_DISPLAYER_H

#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "EglCore.h"
#include "OpenglHelp.h"
#include "VideoDecoder.h"
#include "GLImageFilter.h"
#include "YUVImageFilter.h"
#include "RenderManager.h"

extern "C"{
#include "libavutil/frame.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

typedef double (*getFrameCallback)(AVFrame** frame,void*ctx);

class Displayer{
private:
    char* url;
    unsigned char* data = NULL;
    EGLSurface  surface;
    OpenglHelp openglHandler;
    ANativeWindow* window;
    YUVSpliter spliter;
    Texture* VideoTexture = NULL;
    GLImageFilter* image = NULL;
    YUVImageFilter* yuv = NULL;
    RenderManager* render = NULL;
    std::mutex mMutex;
    std::condition_variable mCond;
    getFrameCallback  callback;
    void *ctx;
    void renerLoop();
    volatile bool inited;
    int width,height;

    void showOpengl(AVFrame* frame);
    void updateTexture(AVFrame* frame);
public:
    void SurfaceCreated(ANativeWindow*window);
    void showFrame(AVFrame* frame);
    void open(int video_width,int video_height,getFrameCallback frameCallback,void*ctx);
    void notify();
    void updateDisPlaySize();
    bool isInited(){return inited;}

};
#endif //PLAYER_DISPLAYER_H
