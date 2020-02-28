//
// Created by Jarryd on 2019/10/30.
//

#ifndef PLAYER_SONICWRAPPER_H
#define PLAYER_SONICWRAPPER_H

#include "sonic.h"

#define BYTE_PER_SAMPLE 2
typedef  unsigned char byte;
class Sonicwrapper{
private:
    sonicStream sonic;
    short *input;
    short *output;
    int maxOutputSample;
    int channels;
    int sample_rate;
public:
    void init(int sample_rate,int channels);
    int processAudio(byte* samples,int numsample,int size,byte** output);
    void setSpeed(float speed);
    void setPitch(float pitch);
    void setRate(float rate);
};
#endif //PLAYER_SONICWRAPPER_H
