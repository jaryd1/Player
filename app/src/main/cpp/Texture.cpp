//
// Created by Jarryd on 2019/11/2.
//
#include "Texture.h"

Texture::Texture() {
    width = 0;
    height = 0;
    pixels = new unsigned char*[MAX_PIXELS];
    linesizes = new int[MAX_PIXELS];
}

Texture::~Texture() {
    delete pixels;
    delete linesizes;
}

int Texture::getWidth()  {
    return width;
}

void Texture::setWidth(int width) {
    Texture::width = width;
}

int Texture::getHeight()  {
    return height;
}

void Texture::setHeight(int height) {
    Texture::height = height;
}

unsigned char* Texture::getPixel(int index)  {
    return pixels[index];
}

int Texture::getLinesize(int index)  {
    return linesizes[index];
}

void Texture::setPixel(int index, unsigned char *pixel) {
    pixels[index] = pixel;
}

void Texture::setLinesize(int index, int value) {
    linesizes[index] = value;
}

int Texture::getFormat()  {
    return format;
}

void Texture::setFormat(int format) {
    Texture::format = format;
}
