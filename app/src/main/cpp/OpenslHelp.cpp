//
// Created by Jarryd on 2019/9/30.
//
#include "OpenslHelp.h"

static void PcmCall(SLAndroidSimpleBufferQueueItf bf,void *contex){
    OpenslHelp *ap = (OpenslHelp *)contex;
    if(!ap || !bf)
    {
        return;
    }

    int size = ap->GetData(ap->getBuffer(),ap->getBufferSize());
    if(size == -1){
        ap->stop();
    }else {
        (*bf)->Enqueue(bf, ap->getBuffer(), size);
    }
}

void OpenslHelp::open(AudioSpec*spec,getPCMCallback pcmCallback,void*ctx) {
    callback = pcmCallback;
    this->ctx = ctx;
    SLresult  ret;
    ret = slCreateEngine(&eng_obj,0,NULL,0,NULL,NULL);
    if(ret != SL_RESULT_SUCCESS){
        LOGE("create sl engine error");
        return;
    }
    ret = (*eng_obj)->Realize(eng_obj,SL_BOOLEAN_FALSE);
    if(ret != SL_RESULT_SUCCESS){
        LOGE("realize sl error");
        return;
    }
    ret =(*eng_obj)->GetInterface(eng_obj,SL_IID_ENGINE,&eng_itf);
    if(ret != SL_RESULT_SUCCESS){
        LOGE("engine itf error");
        return;
    }
    ret = (*eng_itf)->CreateOutputMix(eng_itf,
            &mix,NULL,NULL,NULL);
    if(ret != SL_RESULT_SUCCESS){
        LOGE("create mix error");
        return;
    }

    ret = (*mix)->Realize(mix,SL_BOOLEAN_FALSE);
    if(ret != SL_RESULT_SUCCESS){
        LOGE("realize mix error");
        return;
    }
    SLDataLocator_OutputMix outmix ={SL_DATALOCATOR_OUTPUTMIX,mix};
    SLDataSink audiosink ={&outmix,NULL};
    SLDataLocator_AndroidSimpleBufferQueue buffer =
            {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,10};
    SLuint32  layout;
    switch (spec->channels){
        case 1:
            layout = SL_SPEAKER_FRONT_CENTER;
            break;
        case 2:
            layout = SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT;
            break;
        default:
            LOGE("invaild channles");
            return;
    }

    SLDataFormat_PCM pcm = {
        SL_DATAFORMAT_PCM,
        (SLuint32)spec->channels,
        coverToSLSampleRate(spec->sample_rate),
        SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_PCMSAMPLEFORMAT_FIXED_16,
        layout,
        SL_BYTEORDER_LITTLEENDIAN
    };

    BufferSize =(int)(spec->sample_rate * spec->channels * buffer_second*2);
    LOGE("buffer size %d",BufferSize);
    Buffer = new byte[BufferSize];
    SLDataSource source = {&buffer,&pcm};
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    ret= (*eng_itf)->CreateAudioPlayer(eng_itf,&player_obj,&source,
            &audiosink,sizeof(ids)/sizeof(SLInterfaceID),ids,req);
    if(ret !=SL_RESULT_SUCCESS )
    {
        LOGE("CreateAudioPlayer failed!");
        return;
    }
    (*player_obj)->Realize(player_obj,SL_BOOLEAN_FALSE);
    //获取player接口
    ret = (*player_obj)->GetInterface(player_obj,SL_IID_PLAY,&player_itf);
    if(ret !=SL_RESULT_SUCCESS )
    {
        LOGE("GetInterface SL_IID_PLAY failed!");
        return;
    }
    ret = (*player_obj)->GetInterface(player_obj,SL_IID_BUFFERQUEUE,&queue);
    if(ret !=SL_RESULT_SUCCESS )
    {
        LOGE("GetInterface SL_IID_BUFFERQUEUE failed!");
        return;
    }

    (*queue)->RegisterCallback(queue,PcmCall,this);

    LOGE("init opensl success");
    start();

}

void OpenslHelp::start() {
    //设置为播放状态
    (*player_itf)->SetPlayState(player_itf,SL_PLAYSTATE_PLAYING);

    (*queue)->Enqueue(queue,"",1);
}



SLuint32 OpenslHelp::coverToSLSampleRate(int sampleRate){
    switch (sampleRate) {
        case 8000: {
            return SL_SAMPLINGRATE_8;
        }
        case 11025: {
            return SL_SAMPLINGRATE_11_025;
        }
        case 12000: {
            return SL_SAMPLINGRATE_12;
        }
        case 16000: {
            return SL_SAMPLINGRATE_16;
        }
        case 22050: {
            return SL_SAMPLINGRATE_22_05;
        }
        case 24000: {
            return SL_SAMPLINGRATE_24;
        }
        case 32000: {
            return SL_SAMPLINGRATE_32;
        }
        case 44100: {
            return SL_SAMPLINGRATE_44_1;
        }
        case 48000: {
            return SL_SAMPLINGRATE_48;
        }
        case 64000: {
            return SL_SAMPLINGRATE_64;
        }
        case 88200: {
            return SL_SAMPLINGRATE_88_2;
        }
        case 96000: {
            return SL_SAMPLINGRATE_96;
        }
        case 192000: {
            return SL_SAMPLINGRATE_192;
        }
        default: {
            return SL_SAMPLINGRATE_44_1;
        }
    }
}

void OpenslHelp::togglePlayState(bool playing) {
    if(!player_itf) return;
    if(playing){
        (*player_itf)->SetPlayState(player_itf,SL_PLAYSTATE_PLAYING);
    }else{
        (*player_itf)->SetPlayState(player_itf,SL_PLAYSTATE_PAUSED);
    }
}

