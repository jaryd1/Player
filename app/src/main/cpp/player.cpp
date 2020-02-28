//
// Created by Jarryd on 2019/9/19.
//
#include <jni.h>
extern "C"{
#include "libavcodec/jni.h"
}

#include "Media.h"
Media media;



extern "C"
JNIEXPORT jint
JNICALL
Java_com_c_player_MySurface_play(
        JNIEnv *env,
        jobject) {
   media.togglePlay();
   return 0;
}

extern "C"
JNIEXPORT jint
JNICALL
Java_com_c_player_MySurface_SeekPosition(
        JNIEnv *env,
        jobject obj,
        jfloat position) {
    media.seekPosition(position);
    return 0;
}

extern "C"
JNIEXPORT void
JNICALL
Java_com_c_player_MySurface__1updateDisplaySize(
        JNIEnv *env,
        jobject obj) {
    media.updateDisplaySize();
}


extern "C"
JNIEXPORT jint
JNICALL
Java_com_c_player_MySurface__1setSurface(
        JNIEnv *env,
        jobject obj,
        jobject surface){
    ANativeWindow* window = ANativeWindow_fromSurface(env,surface);
    if(window == NULL){
        LOGE("window error");
    }
    media.onSurfaceCreated(window);
    LOGE("window create success");
    return 0;
}


extern "C"
JNIEXPORT jint
JNICALL
Java_com_c_player_MySurface__1setDataSource(
        JNIEnv *env,
        jobject obj,
        jstring url/* this */){
    const  char *file = env->GetStringUTFChars(url,JNI_FALSE);
    media.url = (char*)av_malloc(strlen(file));
    strcpy(media.url,file);
    env->ReleaseStringUTFChars(url,file);
    LOGE("set data  finish");
    return 0;
}


extern "C"
JNIEXPORT jint
JNICALL
Java_com_c_player_MySurface__1prepareAsync(
        JNIEnv *env,
        jobject obj){
    JavaVM* jvm  = NULL;
    env->GetJavaVM(&jvm);
    jobject  obj1 = env->NewGlobalRef(obj);
    media.initJavaCallBack(jvm,obj1);
    media.startThread();
    return 0;

}

//硬解码关键
extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    av_jni_set_java_vm(vm, 0);
    // 返回jni版本
    return JNI_VERSION_1_4;
}




