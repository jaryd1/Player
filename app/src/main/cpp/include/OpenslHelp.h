//
// Created by Jarryd on 2019/9/30.
//

#ifndef PLAYER_OPENSLHELP_H
#define PLAYER_OPENSLHELP_H

#include "AudioDecoder.h"

extern "C"{
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
#include "androidlog.h"
};

typedef  unsigned char byte;
typedef  unsigned int size_t;
#define buffer_second 0.02

typedef  int(*getPCMCallback)(byte *data,int size,void*ctx);

class OpenslHelp{
private:
SLObjectItf eng_obj = NULL;
SLEngineItf eng_itf = NULL;
SLObjectItf mix = NULL;
SLObjectItf player_obj = NULL;
SLPlayItf   player_itf = NULL;
SLVolumeItf volume = NULL;
SLAndroidSimpleBufferQueueItf queue = NULL;
unsigned  char* Buffer;
int BufferSize;
getPCMCallback  callback;
void*ctx;
    SLuint32 coverToSLSampleRate(int sampleRate);
public:
void open(AudioSpec*spec,getPCMCallback pcmCallback,void*ctx);
byte *getBuffer(){return Buffer;}
int getBufferSize(){return BufferSize;}
int GetData(byte *data,int size){
    return callback(data,size,ctx);
}
void togglePlayState(bool playing);
void stop(){(*player_itf)->SetPlayState(player_itf,SL_PLAYSTATE_STOPPED);}
};
#endif //PLAYER_OPENSLHELP_H
