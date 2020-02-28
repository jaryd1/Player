//
// Created by Jarryd on 2019/10/30.
//
#include <malloc.h>
#include <string.h>
#include <androidlog.h>
#include "Sonicwrapper.h"
void Sonicwrapper::init(int sample_rate, int channels) {
    sonic = sonicCreateStream(sample_rate,channels);
    input = new short[2048];
    this->sample_rate = sample_rate;
    this->channels = channels;
}

int Sonicwrapper::processAudio(byte *samples, int numsample,int size,byte** output_samples) {
    if(sonicGetSpeed(sonic) ==1.0f && sonicGetPitch(sonic) == 1.0f && sonicGetRate(sonic) == 1.0f){
        *output_samples = samples;
        return size;
    }
    for (int i = 0; i < 2048; ++i) {
        input[i] = (samples[i*2] | samples[i*2+1] << 8);
    }
    int ret = sonicWriteShortToStream(sonic,input,numsample);
    if(ret == 0){
        return -1;
    }
    int available = sonicSamplesAvailable(sonic);
    if(available == 0){
        return 0;
    }
    if(available*channels > maxOutputSample){
        maxOutputSample = available*channels;
        output = (short*)realloc(output,maxOutputSample*BYTE_PER_SAMPLE);
        if(output == NULL){
            return -1;
        }
    }
    int result = sonicReadShortFromStream(sonic,output,available);
    result = result*channels*BYTE_PER_SAMPLE;
    if(size < result){
        delete samples;
        *output_samples = new byte[result];
    }else{
        *output_samples = samples;
    }
    memcpy(*output_samples,output,result);
    return result;
}

void Sonicwrapper::setSpeed(float speed) {
    if(sonic != NULL){
        sonicSetSpeed(sonic,speed);
    }
}

void Sonicwrapper::setPitch(float pitch) {
    if(sonic != NULL){
        sonicSetPitch(sonic,pitch);
    }
}

void Sonicwrapper::setRate(float rate) {
    if(sonic != NULL){
        sonicSetRate(sonic,rate);
    }
}
