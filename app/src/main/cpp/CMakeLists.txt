# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
set(FFMPEG_CPP ${CMAKE_SOURCE_DIR}../../../../../ffmpeglib/src/main/cpp)
set(FFMPEG_SRC ${CMAKE_SOURCE_DIR}../../../../../ffmpeglib/src)
set(MAIN ${CMAKE_CURRENT_SOURCE_DIR}../..)
add_library( # Sets the name of the library.
        player

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        VideoDecoder.cpp
        Media.cpp
        player.cpp
        EglCore.cpp
        OpenglHelp.cpp
        OpenslHelp.cpp
        YUVSpliter.cpp
        MediaDocoder.cpp
        AudioDecoder.cpp
        Displayer.cpp
        MediaSyner.cpp
        AllDecoder.cpp
        JavaCallBack.cpp
        MediaClock.cpp
        sonic.c
        Sonicwrapper.cpp
        Matrix.cpp
        Texture.cpp
        GLImageFilter.cpp
        YUVImageFilter.cpp
        RenderManager.cpp
        GLImageVignetteFilter.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.


add_library(
        ffmpeg
        SHARED
        IMPORTED
)
set_target_properties(
        ffmpeg
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SRC}/main/JniLibs/armeabi-v7a/libffmpeg.so
)

include_directories(${MAIN}/cpp/include)
include_directories(${FFMPEG_CPP}/ffmpeg)
include_directories(${FFMPEG_CPP}/include)
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
target_link_libraries( # Specifies the target library.
        player
        ffmpeg
        android
        OpenSLES
        EGL
        GLESv2
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})