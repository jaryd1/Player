//
// Created by Jarryd on 2019/10/25.
//

#ifndef PLAYER_MEDIACALLBACK_H
#define PLAYER_MEDIACALLBACK_H
class MediaCallBack{
public:
    virtual void initDisplayer(int width,int height)=0;
    virtual void initAudio(AudioSpec* spec)=0;
    virtual void displayPreviewFrame(AVFrame* frame)=0;
};
#endif //PLAYER_MEDIACALLBACK_H
