//
// Created by Jarryd on 2019/10/31.
//
#include "GLImageFilter.h"
#include "Matrix.h"

GLImageFilter::GLImageFilter(const char *vexter_shader, const char *fragment_shader) {
    this->VexterShader = vexter_shader;
    this->FragmentShader = fragment_shader;
}

void GLImageFilter::setShaders(char *vexter_shader, char *fragment_shader) {
    this->VexterShader = vexter_shader;
    this->FragmentShader = fragment_shader;
}

void GLImageFilter::setDisplaySize(int width, int height) {
    displayWidth = width;
    displayHeight = height;
//    if(textureWidth !=0 && textureHeight !=0 ) {
//        float texRatio = (float) textureHeight / (float) textureWidth;
//        if (texRatio < 1) {
//            int temp = displayWidth;
//            displayWidth = displayHeight;
//            displayHeight = temp;
//        }
//    }

    updateTexOffset();
}

void GLImageFilter::setTextureSize(int width, int height) {
    textureWidth = width;
    textureHeight = height;
    updateTexOffset();
    initFrameBuffer();
}

void GLImageFilter::init() {
    program = OpenglHelp::createPrograme(VexterShader,FragmentShader);
    initHandlers();
    OpenglHelp::checkError("init");
}

void GLImageFilter::initHandlers() {
    aPositionHandler = glGetAttribLocation(program,"aPosition");
    aTexCoordHandler = glGetAttribLocation(program,"aTexCoord");
    InputTextureHandler = glGetUniformLocation(program,"InputTexture");
    TexMatrixHandler = glGetUniformLocation(program,"uTexMatrix");
    PosMatrixHandler = glGetUniformLocation(program,"uPosMatrix");
    XoffsetHandler = glGetUniformLocation(program,"xoffset");
    YoffsetHandler = glGetUniformLocation(program,"yoffset");
}

void GLImageFilter::initFrameBuffer() {
    if(OffScreenSurface == EGL_NO_SURFACE){
        OffScreenSurface = EglCore::createOffScreenSurface(textureWidth,textureHeight);
    }
    glGenFramebuffers(1,&Fbo);
    initTexture(&outputTexture,NULL);
    glBindFramebuffer(GL_FRAMEBUFFER,Fbo);
    glBindTexture(GL_TEXTURE_2D,outputTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,outputTexture,0);
    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    OpenglHelp::checkError("init frame");
}

void GLImageFilter::initTexture(GLuint *textureId, unsigned char* data) {
    if(*textureId == 0){
        glGenTextures(1,textureId);
        glBindTexture(GL_TEXTURE_2D,*textureId);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        OpenglHelp::checkError("glTexParameteri");
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,textureWidth,textureHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
        glBindTexture(GL_TEXTURE_2D,0);
    }
}

void GLImageFilter::updateTexOffset() {
    if(textureWidth == 0 || displayWidth == 0){
        return;
    }
    float textureAspectRatio = (float)textureHeight /(float) textureWidth;
    float displayAspectRatio = (float)displayHeight /(float) displayWidth;
    if(textureAspectRatio > displayAspectRatio){
       int expectWidth = (int)((float)displayHeight*textureWidth/(float)textureHeight);
       textureXoffset = (float)(displayWidth - expectWidth)/(2*displayWidth);
       if(textureXoffset < 0.1){
           textureXoffset = 0.0;
       }
       textureYoffset = 0.0f;
    }else if(textureAspectRatio < displayAspectRatio){
       int expectHeiht = (int)((float)displayWidth*textureHeight/textureWidth);
       textureYoffset = (float)(displayHeight - expectHeiht)/(2*displayHeight);
       if(textureYoffset < 0.1){
           textureYoffset = 0.0f;
       }
       textureXoffset = 0.0;
    }else{
        textureYoffset = 0.0f;
        textureXoffset = 0.0f;
    }
}


void GLImageFilter::bindAttributes() {
    glEnableVertexAttribArray(aPositionHandler);
    //传递顶点
    glVertexAttribPointer(aPositionHandler,2,GL_FLOAT,GL_FALSE,8 ,vers);//sizeof(float)*3

    glEnableVertexAttribArray(aTexCoordHandler);
    glVertexAttribPointer(aTexCoordHandler,2,GL_FLOAT,GL_FALSE,8 ,txts);//sizeof(float)*2

    float text_matrix[16];
    matrixSetIdentityM(text_matrix);
    glUniformMatrix4fv(TexMatrixHandler,1,GL_FALSE,(GLfloat *)text_matrix);

    float  pos_matrix[16];
    matrixSetIdentityM(pos_matrix);
    glUniformMatrix4fv(PosMatrixHandler,1,GL_FALSE,(GLfloat *)pos_matrix);
}

void GLImageFilter::unbindAttributes() {
    glDisableVertexAttribArray(aPositionHandler);
    glDisableVertexAttribArray(aTexCoordHandler);
}

void GLImageFilter::setupUniforms() {
    glUniform1f(XoffsetHandler,textureXoffset);
    glUniform1f(YoffsetHandler,textureYoffset);
}

void GLImageFilter::setupInputTextures(GLint texture) {
    if(InputTextureHandler !=0 && texture !=0){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        glUniform1i(InputTextureHandler,0);
    }
}

void GLImageFilter::drawFrame(GLuint textureId) {
    glUseProgram(program);
    glViewport(0,0,displayWidth,displayHeight);

    glClearColor(1.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    onDrawTexture(textureId);
    glUseProgram(0);
}

GLuint GLImageFilter::drawFrameBuffer(GLuint textureid) {
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
        LOGE("framebuffer status error 0x%x",status);
    glBindFramebuffer(GL_FRAMEBUFFER,Fbo);
    OpenglHelp::checkError("check bind");
    glUseProgram(program);

    glViewport(0,0,textureWidth,textureHeight);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    setupFrameBufferUniforms();

    onDrawTexture(textureid);
    OpenglHelp::checkError("draw frame draw");
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    OpenglHelp::checkError("draw frame unbind frame");
    glUseProgram(0);

    return outputTexture;
}

void GLImageFilter::onDrawTexture(GLuint textureid) {

    bindAttributes();
    OpenglHelp::checkError("bindAttribute");

    setupInputTextures(textureid);
    setupUniforms();

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    OpenglHelp::checkError("check draw array");

    unbindAttributes();

    glBindTexture(GL_TEXTURE_2D,0);
    OpenglHelp::checkError("unbind text");

}


GLImageFilter::~GLImageFilter() {
    glDeleteProgram(program);
}

EGLSurface GLImageFilter::getOffScreenSurface()  {
    return OffScreenSurface;
}
