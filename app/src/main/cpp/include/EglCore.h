//
// Created by Jarryd on 2019/9/28.
//

#ifndef APP_EGLCORE_H
#define APP_EGLCORE_H
extern "C"{
#include "EGL/egl.h"
#include "GLES2/gl2.h"
#include "androidlog.h"
}
#pragma once
class EglCore{
private:
    static EGLDisplay display;
    static EGLConfig  config;
    static EGLContext context;
public:
    EglCore(){};
    static void init();
    static void swapBuffers(EGLSurface surface);
    static void makeCurrent(EGLSurface surface);
    static void doneCurrent();
    static EGLSurface createWindowSurface(ANativeWindow* window);
    static EGLSurface createOffScreenSurface(int width,int height);
};



#endif //APP_EGLCORE_H
