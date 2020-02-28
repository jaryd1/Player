//
// Created by Jarryd on 2019/10/31.
//

#ifndef PLAYER_GLIMAGEFILTER_H
#define PLAYER_GLIMAGEFILTER_H

#include <OpenglHelp.h>
#include "Texture.h"

class GLImageFilter{
private:
    void updateTexOffset();
protected:
    const char* VexterShader;
    const char* FragmentShader;
    int displayWidth = 0,
        displayHeight = 0;
    EGLSurface OffScreenSurface = EGL_NO_SURFACE;

protected:
    int textureWidth = 0,
    textureHeight = 0;
    float textureXoffset = 0.0f,
    textureYoffset = 0.0f;
    GLuint program;
    GLuint Fbo = 0,outputTexture = 0;
    GLuint test = 0;
    bool frameBufferInited = false;
    float vers[8] = {
            -1.0f,-1.0f,
            1.0f,-1.0f,
            -1.0f,1.0f,
            1.0f,1.0f
    };
//    加入材质坐标数据
    float txts[8] = {
            0.0f,0.0f,
            1.0f,0.0f , //右下
            0.0,1.0,
            1.0f,1.0f
    };

    GLint aPositionHandler =0,
        aTexCoordHandler =0,
        InputTextureHandler =0,
        TexMatrixHandler = 0,
        PosMatrixHandler = 0,
        XoffsetHandler = 0,
        YoffsetHandler = 0;
    virtual void initHandlers();
    virtual void initFrameBuffer();
    virtual void bindAttributes();
    virtual void unbindAttributes();
    virtual void setupInputTextures(GLint inputTexture);
    virtual void setupUniforms();
    virtual void setupFrameBufferUniforms(){};
    virtual void uploadTexture(Texture* texture){};
    void initTexture(GLuint *textureId, unsigned char* data);
    void onDrawTexture(GLuint textureid);
public:
    GLImageFilter(){};
    virtual ~GLImageFilter();
    GLImageFilter( const char*vexter_shader,const  char*fragment_shader);
    void setShaders(char* vexter_shader,char*fragment_shader);
    void setDisplaySize(int width, int height);
    void init();
    void drawFrame(GLuint textureId);
    void setTextureSize(int width, int height);
    GLuint drawFrameBuffer(GLuint textureid);
    EGLSurface getOffScreenSurface() ;

};
#endif //PLAYER_GLIMAGEFILTER_H
