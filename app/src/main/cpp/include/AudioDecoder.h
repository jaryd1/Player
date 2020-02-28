//
// Created by Jarryd on 2019/9/30.
//

#ifndef PLAYER_AUDIODECODER_H
#define PLAYER_AUDIODECODER_H

#include "MediaDecoder.h"
#include "AudioSpec.h"
#include "sonic.h"
#include "Sonicwrapper.h"

extern "C"{
#include <libswresample/swresample.h>
};

typedef unsigned char byte;

class AudioFrame{
private:
    double pts;
    byte *data;
    int nb_samples;
    int size;
    double duration;

public:
    ~AudioFrame();

public:
    int getSize() ;

    void setSize(int size);

    double getPts() ;

    void setPts(double pts);

    byte *getData() ;

    void setData(byte *data);

    int getNbSamples() ;

    void setNbSamples(int nbSamples);

    double getDuration() ;

    void setDuration(double duration);


};

class AudioDecoder: public MediaDecoder{

public:
    AudioFrame * getPCM();
    void putPCM(AudioFrame* frame);
    AudioFrame* coverdata(AVFrame* frame);
    int getPCMSize(){return pcms.size();}
    void clearBuffer() override ;
    AudioSpec* getAudioSpec();
private:
    int datasize = 0;
    SwrContext* swrContext = NULL;
    AudioSpec* spec = NULL;
    std::list<AudioFrame*>pcms;
    uint8_t *getOutputBuffer(AVFrame* frame);
    void initAudioSpec();
};
#endif //PLAYER_AUDIODECODER_H
