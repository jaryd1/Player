//
// Created by Jarryd on 2019/10/25.
//

#ifndef PLAYER_MEDIACALLBACK_H
#define PLAYER_MEDIACALLBACK_H
class MediaCallBack{
public:
    virtual void initDisplayer(int width,int height){};
    virtual void initAudio(AudioSpec* spec){};
};
#endif //PLAYER_MEDIACALLBACK_H
