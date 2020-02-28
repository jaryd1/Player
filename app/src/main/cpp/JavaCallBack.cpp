//
// Created by Jarryd on 2019/10/15.
//
#include "JavaCallBack.h"
#include "androidlog.h"
void JavaCallBack::init(JavaVM *jvm, jobject obj) {
    this->g_jvm = jvm;
    this->obj = obj;
}

void JavaCallBack::Call(const char *name, const char *signature, ...) {
    JNIEnv* env;
    if(g_jvm->AttachCurrentThread(&env,NULL) !=JNI_OK){
        LOGE("java call attach error");
        return;
    }
    if(env == NULL){
        LOGE("get env error");
        return;
    }
    jclass  cla = env->GetObjectClass(obj);
    if(cla != NULL){
        jmethodID  id =env->GetMethodID(cla,name,signature);
        if(id != NULL){
            va_list arg;
            va_start(arg,signature);
            env->CallVoidMethodV(obj,id,arg);
            va_end(arg);
        }
        env->DeleteLocalRef(cla);
    }
    g_jvm->DetachCurrentThread();
}

void JavaCallBack::callDuration(float duration) {
    Call("updataDuration","(F)V",duration);
}

void JavaCallBack::callCurrentPos(long cur, float buff,long dur) {
    Call("call","(JFF)V",(jlong)cur,(jfloat)buff,(jfloat)dur);
}

void JavaCallBack::showLoading() {
    Call("showLoading","()V");
}

void JavaCallBack::hideLoading() {
    Call("hideLoading","()V");
}

void JavaCallBack::setVideoSize(int width, int height) {
    Call("getVideoSize","(II)V",(jint)width,(jint)height);
}