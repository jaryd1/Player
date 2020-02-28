//
// Created by Jarryd on 2019/11/2.
//

#ifndef PLAYER_TEXTURE_H
#define PLAYER_TEXTURE_H

#define MAX_PIXELS 3

enum FORMAT{
    YUV420P = 0,
    RGBA = 1,
    NV21 =25,
    NV12 = 26
};

class Texture{
private:
    int width;
    int height;
    int format;
    unsigned char** pixels;
    int* linesizes;
public:
    Texture();
    ~Texture();

    int getFormat() ;

    void setFormat(int format);

    int getWidth() ;

    void setWidth(int width);

    int getHeight() ;

    void setHeight(int height);

    unsigned char *getPixel(int index) ;

    void setPixel(int index, unsigned char* pixel) ;

     int getLinesize(int index) ;

     void setLinesize(int index,int value);


};
#endif //PLAYER_TEXTURE_H
