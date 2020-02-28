//
// Created by Jarryd on 2019/10/25.
//

#ifndef PLAYER_AUDIOSPEC_H
#define PLAYER_AUDIOSPEC_H
class AudioSpec{
public:
    uint8_t channels = 0;
    uint64_t channel_layout =0;
    int byte_per_sample = 0;
    int sample_rate = 0;
    int getBytePS(){return channels*sample_rate*byte_per_sample;}
};
#endif //PLAYER_AUDIOSPEC_H
