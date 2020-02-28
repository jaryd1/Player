//
// Created by Jarryd on 2019/11/3.
//
#include "GLImageVignetteFilter.h"

GLImageVignetteFilter::GLImageVignetteFilter(const char *vertex,
        const char *fragment):GLImageFilter(vertex,fragment) {

}

void GLImageVignetteFilter::initHandlers() {
    GLImageFilter::initHandlers();
    CenterHandler = glGetUniformLocation(program,"vignetteCenter");
    ColorHandler = glGetUniformLocation(program,"vignetteColor");
    StartHandler = glGetUniformLocation(program,"vignetteStart");
    EndHandler = glGetUniformLocation(program,"vignetteEnd");
    center_points = new GLfloat[2]{0.5f,0.5f};
    center_colors = new GLfloat[3] {0.0f,0.0f,0.0f};
}

void GLImageVignetteFilter::setupFrameBufferUniforms() {
    glUniform1f(StartHandler,0.0f);
    glUniform1f(EndHandler,0.5f);
    glUniform2fv(CenterHandler,1,center_colors);
}
