//
// Created by Jarryd on 2019/11/2.
//
#include "RenderManager.h"

RenderManager::RenderManager() {
    displayFilter = new GLImageFilter(OpenglHelp::getShader(0),OpenglHelp::getShader(1));
    inputFilter = new  YUVImageFilter(OpenglHelp::getShader(3),OpenglHelp::getShader(2));
}

RenderManager::~RenderManager() {
    delete displayFilter;
    delete inputFilter;
}

void RenderManager::init(int displayWidth, int displayHeight, int textureWidth,
                         int textureHeight) {
    displayFilter->init();
    inputFilter->init();
    displayFilter->setDisplaySize(displayWidth, displayHeight);
    inputFilter->setDisplaySize(displayWidth, displayHeight);
    displayFilter->setTextureSize(textureWidth,textureHeight);
    inputFilter->setTextureSize(textureWidth, textureHeight);

}

void RenderManager::updateDisplaySize(int width, int height) {
    if(displayFilter != NULL){
        displayFilter->setDisplaySize(width,height);
    }
}

void RenderManager::updateTextureSize(int width, int height) {
    if(inputFilter != NULL){
        inputFilter->setTextureSize(width,height);
    }
}

void RenderManager::renderToWindow(Texture *texture, EGLSurface window) {

    EglCore::makeCurrent(inputFilter->getOffScreenSurface());
    inputFilter->uploadTexture(texture);
    int out = inputFilter->drawFrameBuffer(0);
    EglCore::swapBuffers(inputFilter->getOffScreenSurface());

//    EglCore::makeCurrent(filter->getOffScreenSurface());
//    int out2 = filter->drawFrameBuffer(out);
//    EglCore::swapBuffers(filter->getOffScreenSurface());

    EglCore::makeCurrent(window);
//    inputFilter->uploadTexture(texture);
//    inputFilter->drawFrame(0);
    displayFilter->drawFrame(out);
    EglCore::swapBuffers(window);
}
