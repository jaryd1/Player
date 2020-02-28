//
// Created by Jarryd on 2019/9/28.
//
#include <android/native_window.h>
#include "EglCore.h"

EGLDisplay  EglCore::display = EGL_NO_DISPLAY;
EGLConfig   EglCore::config = NULL;
EGLContext  EglCore::context = EGL_NO_CONTEXT;

void EglCore::init() {

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display == nullptr){
        LOGE("init display error");
        return;
    }
    if(eglInitialize(display,0,0) != EGL_TRUE){
        LOGE("init error");
        return;
    }
    EGLint configSpec [] = {
            EGL_RED_SIZE,8,
            EGL_GREEN_SIZE,8,
            EGL_BLUE_SIZE,8,
            EGL_SURFACE_TYPE,EGL_WINDOW_BIT,
            EGL_NONE
    };
    EGLConfig  eglConfig = NULL;
    EGLint configNums = 0;
    if(eglChooseConfig(display,configSpec,&eglConfig,1,&configNums) !=EGL_TRUE){
        LOGE("config error");
        return;
    }

    EGLint attrs[] = {EGL_CONTEXT_CLIENT_VERSION ,2, EGL_NONE};
    context = eglCreateContext(display,eglConfig, NULL,attrs);
    if(context == EGL_NO_CONTEXT){
        LOGE("context error");
        return;
    }
    EglCore::config = eglConfig;
    LOGE("init opengl success");
}

EGLSurface EglCore::createWindowSurface(ANativeWindow *window) {
    if(config == NULL || display == EGL_NO_DISPLAY || context == EGL_NO_CONTEXT || window == NULL){
        return NULL;
    }
    EGLint format;
    if(!(eglGetConfigAttrib(display,config,EGL_NATIVE_VISUAL_ID,&format))){
        LOGE("get egl configattrib error");
        return NULL;
    }
    ANativeWindow_setBuffersGeometry(window,0,0,format);
    EGLSurface surface = eglCreateWindowSurface(display,config,window,NULL);
    if(!surface){
        LOGE("creat window surface error");
        return NULL;
    }
    LOGE("create window surface success");
    return surface;
}

EGLSurface EglCore::createOffScreenSurface(int width, int height) {
    EGLint attribs[] ={EGL_WIDTH,width,EGL_HEIGHT,height,EGL_NONE,EGL_NONE};
    EGLSurface  surface = eglCreatePbufferSurface(display,config,attribs);
    if(!surface){
        LOGE("create surface error");
        return NULL;
    }
    return surface;
}

void EglCore::makeCurrent(EGLSurface surface) {
    if(display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE
    || context == EGL_NO_CONTEXT){
        LOGE("not init");
        if(display == EGL_NO_DISPLAY){
            LOGE("no display");
        }
        if(context == EGL_NO_CONTEXT){
            LOGE("no context");
        }
        if(surface == EGL_NO_SURFACE){
            LOGE("no surface");
        }
        return;
    }
    if((eglMakeCurrent(display,surface,surface,context) != EGL_TRUE)){
        LOGE("make current faild");
    }
}

void EglCore::doneCurrent() {
    if(eglMakeCurrent(display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT) !=
    EGL_TRUE){
        LOGE("done current faild");
    }
}

void EglCore::swapBuffers(EGLSurface surface) {
    if(display == EGL_NO_DISPLAY ||
        surface == EGL_NO_SURFACE){
        LOGE("not init");
    }
    eglSwapBuffers(display,surface);
    doneCurrent();
}
