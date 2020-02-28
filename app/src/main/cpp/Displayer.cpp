//
// Created by Jarryd on 2019/9/29.
//
#include <thread>
#include "Displayer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
extern "C"{
#include <libavutil/time.h>
}

void Displayer::SurfaceCreated(ANativeWindow *window) {
    this->window = window;
    if(callback && ctx){
        open(width,height,callback,ctx);
    }
}

void Displayer::open(int width,int height,getFrameCallback frameCallback,void*ctx) {
        callback = frameCallback;
        this->width = width;
        this->height = height;
        this->ctx = ctx;
        url = "/storage/emulated/0/tu.jpg";
        if(window == NULL){
            return;
        }
        EglCore::init();
        surface = EglCore::createWindowSurface(window);
        EglCore::makeCurrent(surface);
        render = new RenderManager();
        render->init(ANativeWindow_getWidth(window),ANativeWindow_getHeight(window),
                width,height);
        EglCore::doneCurrent();
        inited = 1;
        VideoTexture  = new Texture();
        std::thread render(&Displayer::renerLoop,this);
        render.detach();
}


void Displayer::renerLoop() {
    while(true) {
        AVFrame *frame;
        if(callback == NULL){
            LOGE("call back null");
        }
        if(ctx == NULL){
            LOGE("ctx null");
        }
        double sleep_time = callback(&frame,ctx);
        if(sleep_time == -1){
            break;
        }
        av_usleep(sleep_time*1000000);
        if(frame) {
            showFrame(frame);
        }
    }
    LOGE("render finish");
}

void Displayer::notify() {
    mCond.notify_all();
}

void Displayer::updateDisPlaySize() {
  if(render != NULL){
      render->updateDisplaySize(ANativeWindow_getWidth(window),ANativeWindow_getHeight(window));
  }
}


void Displayer::updateTexture(AVFrame *frame) {
    spliter.spliter(frame);
    VideoTexture->setPixel(0,spliter.y);
    VideoTexture->setPixel(1,spliter.u);
    VideoTexture->setPixel(2,spliter.v);
    VideoTexture->setLinesize(0,frame->linesize[0]);
    VideoTexture->setLinesize(1,frame->linesize[1]);
    VideoTexture->setLinesize(2,frame->linesize[2]);
    VideoTexture->setFormat(frame->format);
    VideoTexture->setHeight(frame->height);
    VideoTexture->setWidth(frame->width);
}

void Displayer::showFrame(AVFrame *frame) {
    updateTexture(frame);
    render->renderToWindow(VideoTexture,surface);
    av_frame_free(&frame);
}



void Displayer::showOpengl(AVFrame *frame) {
    EglCore::makeCurrent(surface);
    spliter.spliter(frame);
    openglHandler.Draw(frame->width, frame->height, spliter,
                       frame->format);
    EglCore::swapBuffers(surface);
    av_frame_free(&frame);
}