//
// Created by Jarryd on 2019/10/15.
//

#ifndef PLAYER_JAVACALLBACK_H
#define PLAYER_JAVACALLBACK_H

#include <jni.h>

class JavaCallBack{
private:
    JavaVM *g_jvm;
    jobject obj;
    void Call(const char* name, const char*sig,...);
public:
    void init(JavaVM* jvm,jobject obj);
    void callDuration(float duration);
    void callCurrentPos(long progress,float bufferprogress,long duration);
    void showLoading();
    void hideLoading();
    void setVideoSize(int width,int height);
};
#endif //PLAYER_JAVACALLBACK_H
