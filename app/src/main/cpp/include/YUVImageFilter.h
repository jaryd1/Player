//
// Created by Jarryd on 2019/10/31.
//

#ifndef PLAYER_YUVIMAGEFILTER_H
#define PLAYER_YUVIMAGEFILTER_H

#include "GLImageFilter.h"

class YUVImageFilter: public GLImageFilter{
private:
    GLint FormatTypeHandler,*YUVHandlers;
    GLuint *textures = NULL;
    int format;
    void setupFrameBufferUniforms() override ;
public:
    YUVImageFilter():GLImageFilter(){};
    YUVImageFilter(const char* vs, const char* fs);
    ~YUVImageFilter();
    void initHandlers() override ;
    void initFrameBuffer() override ;
    GLuint drawYUV(YUVSpliter spliter,int avPiexlformat);
    void uploadTexture(Texture* texture) override ;
    void renderTexture();
};

#endif //PLAYER_YUVIMAGEFILTER_H
