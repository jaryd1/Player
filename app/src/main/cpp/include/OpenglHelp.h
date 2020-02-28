//
// Created by Jarryd on 2019/9/28.
//

#ifndef APP_OPENGLHANDLER_H
#define APP_OPENGLHANDLER_H

#include "YUVSpliter.h"
#include "EglCore.h"

#define toString(x) #x

class OpenglHelp{
private:
    const char* vertexShader;
    const char* fragmentShader;
    GLuint program;
    float vers[12] = {
            1.0f,-1.0f,0.0f,
            -1.0f,-1.0f,0.0f,
            1.0f,1.0f,0.0f,
            -1.0f,1.0f,0.0f,
    };

    //加入材质坐标数据
    float txts[8] = {
            1.0f,0.0f , //右下
            0.0f,0.0f,
            1.0f,1.0f,
            0.0,1.0
    };
    GLuint aPositionLocation , aTexCoordLocation;
    GLuint yTextureLocation , uTextureLocation,vTextureLocation;
    GLuint yuvTexture[3]={0};
    GLuint width , height;
    GLuint formatTypeLocation;

    void setUniform();
//    void GetTexture(unsigned int index,int width,int height, unsigned char *buf, bool isa);
public:
    static GLuint createPrograme(const char* vertex_shader, const char* fragment_shader);
    static GLuint compileShader(GLenum type, const char* Shader);
    OpenglHelp(){};
    void init();
    void Draw(int width , int height, YUVSpliter spliter,int avPixelFormat);
    void GetTexture(unsigned int index,int width,int height, unsigned char *buf, bool isa);
    static void checkError(char* opt);
    static const char* getShader(int n);
};
#endif //APP_OPENGLHANDLER_H
