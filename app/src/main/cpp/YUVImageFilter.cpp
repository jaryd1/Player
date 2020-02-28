//
// Created by Jarryd on 2019/10/31.
//
#include "YUVImageFilter.h"

YUVImageFilter::YUVImageFilter(const char *vs, const char *fs):GLImageFilter(vs,fs) {
    YUVHandlers = new GLint[3];
}

void YUVImageFilter::initHandlers() {
    GLImageFilter::initHandlers();
    YUVHandlers[0] = glGetUniformLocation(program, "yTexture");
    YUVHandlers[1] = glGetUniformLocation(program, "uTexture");
    YUVHandlers[2] = glGetUniformLocation(program, "vTexture");
    FormatTypeHandler = glGetUniformLocation(program, "formatType");
}

void YUVImageFilter::initFrameBuffer() {
    GLImageFilter::initFrameBuffer();
    textures = new GLuint[3]{0,0,0};
    glGenTextures(3,textures);
    int width[] = {textureWidth,textureWidth >> 1,textureWidth >>1};
    int height[] = {textureHeight,textureHeight >> 1,textureHeight >> 1};

    for (int i = 0; i < 3; ++i) {
        glGenTextures(1,&textures[i]);
        glBindTexture(GL_TEXTURE_2D,textures[i]);
        OpenglHelp::checkError("BIndtexture");
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        OpenglHelp::checkError("glTexParameteri");
        glBindTexture(GL_TEXTURE_2D,0);
    }

}




GLuint YUVImageFilter::drawYUV(YUVSpliter spliter, int avPiexlformat) {
//    EglCore::makeCurrent(OffScreenSurface);

//    initTexture(&outputTexture,NULL);


//    uploadTexture(videoTexture);
//    glBindTexture(GL_TEXTURE_2D,outputTexture);
    glBindFramebuffer(GL_FRAMEBUFFER,Fbo);

//    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,outputTexture,0);
    OpenglHelp::checkError("bind framebuff");
//
    OpenglHelp::checkError("init output");
    glViewport(0,0,textureWidth,textureHeight);
    glUseProgram(program);
    OpenglHelp::checkError("use program");

    setupFrameBufferUniforms();
    onDrawTexture(0);
//    bindAttributes();
//
//    OpenglHelp::checkError("set uniform framebuffer");
//
//    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
//    OpenglHelp::checkError("draw triangel");
//
//    glDisableVertexAttribArray(aPositionHandler);
//    glDisableVertexAttribArray(aTexCoordHandler);
//    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
//    EglCore::swapBuffers(OffScreenSurface);
    return outputTexture;
}

void YUVImageFilter::uploadTexture(Texture *texture) {
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    int heights[] = {texture->getHeight(),texture->getHeight() >> 1,texture->getHeight()>>1};
    int width[] = {texture->getWidth(),texture->getWidth() >> 1,texture->getWidth()>>1};
    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D,textures[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,width[i],heights[i],0,GL_LUMINANCE,
                GL_UNSIGNED_BYTE,texture->getPixel(i));

    }
    format = texture->getFormat();
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    OpenglHelp::checkError("upload");
}

void YUVImageFilter::setupFrameBufferUniforms() {
    glUniform1i(YUVHandlers[0],0);
    glUniform1i(YUVHandlers[1],1);
    glUniform1i(YUVHandlers[2],2);
    glUniform1i(FormatTypeHandler,format);
}

void YUVImageFilter::renderTexture() {
    glUseProgram(program);
    glViewport(0,0,displayWidth,displayHeight);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    bindAttributes();
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    glDisableVertexAttribArray(aPositionHandler);
    glDisableVertexAttribArray(aTexCoordHandler);
    glBindTexture(GL_TEXTURE_2D,0);
}



YUVImageFilter::~YUVImageFilter() {
    GLImageFilter::~GLImageFilter();

}