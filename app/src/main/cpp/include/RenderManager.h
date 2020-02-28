//
// Created by Jarryd on 2019/11/2.
//

#ifndef PLAYER_RENDERMANAGER_H
#define PLAYER_RENDERMANAGER_H

#include "GLImageFilter.h"
#include "YUVImageFilter.h"
#include "GLImageVignetteFilter.h"
typedef unsigned  char byte;

class RenderManager{
private:
    const char* url ="/storage/emulated/0/tttt.yuv";
    byte *test,*y,*u,*v;
    GLImageFilter* displayFilter;
    YUVImageFilter* inputFilter;
    GLImageVignetteFilter* filter;
    bool fullscreen = false;
public:
    RenderManager();
    ~RenderManager();
    void init(int displayWidth,int displayHeight,int textureWidth,int textureHeight);
    void updateDisplaySize(int width,int height);
    void updateTextureSize(int width,int height);
    void renderToWindow(Texture* texture,EGLSurface window);
    void setFullscreen(bool isfullscreen){fullscreen = isfullscreen;}

};
#endif //PLAYER_RENDERMANAGER_H
