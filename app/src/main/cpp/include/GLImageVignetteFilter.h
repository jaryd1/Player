//
// Created by Jarryd on 2019/11/3.
//

#ifndef PLAYER_GLIMAGEVIGNETTEFILTER_H
#define PLAYER_GLIMAGEVIGNETTEFILTER_H

#include "GLImageFilter.h"

class GLImageVignetteFilter: public GLImageFilter{
private:
    GLint CenterHandler, ColorHandler,StartHandler,EndHandler;
    GLfloat *center_points,*center_colors;
    void initHandlers() override ;
    void setupFrameBufferUniforms() override ;

public:
    GLImageVignetteFilter(const char* vertex, const char* fragment);
};
#endif //PLAYER_GLIMAGEVIGNETTEFILTER_H
