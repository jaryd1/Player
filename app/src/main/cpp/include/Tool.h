//
// Created by Jarryd on 2019/10/28.
//

#ifndef PLAYER_TOOL_H
#define PLAYER_TOOL_H

typedef signed short SInt16;
typedef unsigned char byte;


inline void converttobytearray(SInt16 source, byte* bytes2) {
    bytes2[0] = (byte) (source & 0xff);
    bytes2[1] = (byte) ((source >> 8) & 0xff);
}


inline SInt16 convertshort(byte* bytes) {
    return (bytes[0] << 8) + (bytes[1] & 0xFF);
}

inline SInt16 adjustAudioVolume(SInt16 source, float volume) {
    SInt16 result = source;
    int temp = (int) ((int) source * volume);
    if (temp < -0x8000) {
        result = -0x8000;
    } else if (temp > 0x7FFF) {
        result = 0x7FFF;
    } else {
        result = (short) temp;
    }
    return result;
}

inline void convertShortArrayFromByteArray(byte *bytearray, int size, SInt16 *shortarray, float audioVolume) {
    byte* bytes = new byte[2];
    for (int i = 0; i < size / 2; i++) {
        bytes[1] = bytearray[2 * i];
        bytes[0] = bytearray[2 * i + 1];
        SInt16 source = convertshort(bytes);
        if (audioVolume != 1.0) {
            shortarray[i] = adjustAudioVolume(source, audioVolume);
        } else {
            shortarray[i] = source;
        }
    }
    delete[] bytes;
}

inline void convertByteArrayFromShortArray(SInt16 *shortarray, int size, byte *bytearray) {
    byte* tmpbytearray = new byte[2];
    for (int i = 0; i < size; i++) {
        converttobytearray(shortarray[i], tmpbytearray);
        bytearray[i * 2] = tmpbytearray[0];
        bytearray[i * 2 + 1] = tmpbytearray[1];
    }
    delete[] tmpbytearray;
}


#endif //PLAYER_TOOL_H
