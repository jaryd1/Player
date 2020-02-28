//
// Created by Jarryd on 2019/9/28.
//
#include <GLES2/gl2.h>
#include "OpenglHelp.h"
static const char* VERTEXSHADER = toString(
        attribute vec4 aPosition; //顶点坐标
        attribute vec2 aTexCoord; //材质顶点坐标
        varying vec2 vTexCoord;   //输出的材质坐标
        void main(){
                vTexCoord = vec2(aTexCoord.x,1.0-aTexCoord.y);
                gl_Position = aPosition;
        }
        );
static const char* YUVSPSHADER = toString(
        precision mediump float;    //精度
        varying vec2 vTexCoord;     //顶点着色器传递的坐标
        uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;
        uniform int formatType;
        void main(){
                vec3 yuv;
                vec3 rgb;
                if(formatType==0)//yuv420p
                {
                        yuv.r = texture2D(yTexture,vTexCoord).r;
                        yuv.g = texture2D(uTexture,vTexCoord).r - 0.5;//
                        yuv.b = texture2D(vTexture,vTexCoord).r - 0.5;
                }else if(formatType==25)//nv12
                {
                        yuv.r = texture2D(yTexture,vTexCoord).r;
                        yuv.g = texture2D(uTexture,vTexCoord).r - 0.5 ;
                        // shader 会将数据归一化，而 uv 的取值区间本身存在-128到正128 然后归一化到0-1 为了正确计算成rgb，
                        // 则需要归一化到 -0.5 - 0.5的区间
                        yuv.b = texture2D(uTexture,vTexCoord).a - 0.5;
                }else if(formatType==26)//nv21
                {
                        yuv.r = texture2D(yTexture,vTexCoord).r;
                        yuv.g = texture2D(uTexture,vTexCoord).a - 0.5 ;
                        // shader 会将数据归一化，而 uv 的取值区间本身存在-128到正128 然后归一化到0-1 为了正确计算成rgb，
                        // 则需要归一化到 -0.5 - 0.5的区间
                        yuv.b = texture2D(uTexture,vTexCoord).r - 0.5;
                }
                rgb = mat3(1.0,     1.0,    1.0,
                           0.0,-0.39465,2.03211,
                           1.13983,-0.58060,0.0)*yuv;

                //输出像素颜色
                gl_FragColor = vec4(rgb,1.0);
        }
        );

GLuint OpenglHelp::compileShader(GLenum type, const char *Shader) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1 , &Shader, nullptr);//1 表示多少句代码，所有代码都放着。第三个表示如果多句代码，
        //则要与前面的代码的长度。
        glCompileShader(shader);

        GLint compileResult = GL_TRUE;
        // 查看编译状态
        glGetShaderiv(shader,GL_COMPILE_STATUS, &compileResult);
        if(compileResult == GL_FALSE){
                char szLog[1024] = {0};
                GLsizei  logLen = 0;// 存储日志长度
                glGetShaderInfoLog(shader , 1024 , &logLen , szLog);//1024 为日志的最大长度
                LOGE("compile error , log: %s" , szLog);
                LOGE("compile error ,shader %s" , Shader);
                glDeleteShader(shader);
                return -1;
        }
        return shader;
}

GLuint OpenglHelp::createPrograme(const char *vertex_shader, const char *fragment_shader) {
    GLuint program = glCreateProgram();
    GLuint vs = OpenglHelp::compileShader(GL_VERTEX_SHADER,vertex_shader);
    GLuint fs = OpenglHelp::compileShader(GL_FRAGMENT_SHADER,fragment_shader);
    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    GLint nResult;
    glGetProgramiv(program , GL_LINK_STATUS, &nResult);
    if(nResult == GL_FALSE){
        char log[1024] = {0};
        GLsizei  len = 0;// 存储日志长度
        glGetShaderInfoLog(program , 1024 , &len , log);//1024 为日志的最大长度
        LOGE("create program error , log: %s" , log);
        glDeleteProgram(program);
        return  -1;
    }
    LOGE("create program success program = %d ",program );
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void OpenglHelp::init() {
    vertexShader = VERTEXSHADER;
    fragmentShader = YUVSPSHADER;

    program = createPrograme(vertexShader,fragmentShader);
    LOGE("CreateProgram create success");

    aPositionLocation = (GLuint)glGetAttribLocation(program,"aPosition");
    aTexCoordLocation = (GLuint)glGetAttribLocation(program,"aTexCoord");

    yTextureLocation = glGetUniformLocation(program, "yTexture");
    uTextureLocation = glGetUniformLocation(program, "uTexture");
    vTextureLocation = glGetUniformLocation(program, "vTexture");

    formatTypeLocation = glGetUniformLocation(program, "formatType");
}

void OpenglHelp::setUniform()
{

    glEnableVertexAttribArray(aPositionLocation);
    //传递顶点
    glVertexAttribPointer(aPositionLocation,3,GL_FLOAT,GL_FALSE,12 ,vers);//sizeof(float)*3

    glEnableVertexAttribArray(aTexCoordLocation);
    glVertexAttribPointer(aTexCoordLocation,2,GL_FLOAT,GL_FALSE,8 ,txts);//sizeof(float)*2

}

void OpenglHelp::GetTexture(unsigned int index, int width, int height, unsigned char *buf,
                               bool isa) {
    unsigned int format =GL_LUMINANCE; // 这里是 灰度图，单通道
    if(isa)
        format = GL_LUMINANCE_ALPHA; // 这表示的是 带 alpha通道的单通道图，即 2通道，r 和 a

    if(yuvTexture[index] == 0)
    {
        //材质初始化
        glGenTextures(1,&yuvTexture[index]);
        LOGE("创建纹理yuvTexture[index] = %d ", yuvTexture[index]);

        //设置纹理属性
        glBindTexture(GL_TEXTURE_2D,yuvTexture[index]);
        //缩小的过滤器
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        //设置纹理的格式和大小
        glTexImage2D(GL_TEXTURE_2D,
                     0,           //细节基本 0默认
                     format,//gpu内部格式 亮度，灰度图
                     width,height, //拉升到全屏
                     0,             //边框
                     format,//数据的像素格式 亮度，灰度图 要与上面一致
                     GL_UNSIGNED_BYTE, //像素的数据类型
                     NULL                    //纹理的数据
        );
    }


    //激活第1层纹理,绑定到创建的opengl纹理
    glActiveTexture(GL_TEXTURE0+index);
    glBindTexture(GL_TEXTURE_2D,yuvTexture[index]);
    //替换纹理内容
    if(width % 4 !=0){
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    }
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,format,GL_UNSIGNED_BYTE,buf);
}

void OpenglHelp::Draw(int width, int height,
                         YUVSpliter spliter, int avPixelFormat) {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    glViewport(0,0, 1920, 1080);
    glUseProgram(program);
    setUniform();
    glUniform1i(formatTypeLocation, avPixelFormat);

    if(avPixelFormat == AV_PIX_FMT_YUV420P)
    {
        GetTexture(0,width,height,spliter.y, false);  // Y
        glUniform1i(yTextureLocation,0);
        GetTexture(1,width/2,height/2,spliter.u,false);  // U
        glUniform1i(uTextureLocation,1);
        GetTexture(2,width/2,height/2,spliter.v,false);  // V
        glUniform1i(vTextureLocation,2);
    }
    else if(avPixelFormat == AV_PIX_FMT_NV12 || avPixelFormat == AV_PIX_FMT_NV21)
    {
        GetTexture(0,width,height,spliter.y,false);  // Y
        glUniform1i(yTextureLocation,0);
        GetTexture(1,width/2,height/2,spliter.u,true);  // Uv
        glUniform1i(uTextureLocation,1);
    }


    //三维绘制
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);

    glDisableVertexAttribArray(aPositionLocation);
    glDisableVertexAttribArray(aTexCoordLocation);
}


void OpenglHelp::checkError(char *opt) {
    GLuint  error;
    for(error = glGetError();error;error = glGetError()){
        LOGE("opt %s,error 0x%x",opt,error);
    }
}

const char * OpenglHelp::getShader(int n) {
    switch (n){
        case 0:
            return toString(
                    attribute vec4 aPosition; //顶点坐标
                    attribute vec4 aTexCoord; //材质顶点坐标
                    varying vec2 vTexCoord;   //输出的材质坐标
                    uniform mat4 uTexMatrix;
                    uniform mat4 uPosMatrix;
                    void main(){
                        vTexCoord = (uTexMatrix * aTexCoord).xy;
                        gl_Position = uPosMatrix*aPosition;
                    }
                    );
        case 1:
            return toString(
                    precision highp float;    //精度
                    varying vec2 vTexCoord;
                    uniform sampler2D inputTexture;
                    uniform float yoffset;
                    uniform float xoffset;
                    void main(){
                        vec2 xy = vTexCoord;
                        vec4 col = vec4(0.0,0.0,0.0,1.0);
                        if(xy.x >= xoffset && xy.x <= (1.0-xoffset) &&
                            xy.y >= yoffset && xy.y <= (1.0 - yoffset)){
                            xy.x -= xoffset;
                            xy.x /= (1.0-2.0*xoffset);
                            xy.y -= yoffset;
                            xy.y /= (1.0 - 2.0*yoffset);
                            col = texture2D(inputTexture,xy);
                        }

                        gl_FragColor = col;
                    }
                    );
        case 2:
            return YUVSPSHADER;
        case 3:
            return VERTEXSHADER;
        case 4:
            return toString(
                    precision mediump float;
                    uniform sampler2D inputTexture;
                    varying vec2 vTexCoord;

                    uniform lowp vec2 vignetteCenter;
                    uniform lowp vec3 vignetteColor;
                    uniform highp float vignetteStart;
                    uniform highp float vignetteEnd;
                    void main()
                    {
                        lowp vec3 rgb = texture2D(inputTexture, vTexCoord).rgb;
                        lowp float d = distance(vTexCoord,
                                                vec2(vignetteCenter.x, vignetteCenter.y));
                        lowp float percent = smoothstep(vignetteStart, vignetteEnd, d);
                        gl_FragColor = vec4(mix(rgb.x, vignetteColor.x, percent),
                                            mix(rgb.y, vignetteColor.y, percent),

                                            mix(rgb.z, vignetteColor.z, percent), 1.0);
                    }
                    );

    }
}