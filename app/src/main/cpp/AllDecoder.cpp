//
// Created by Jarryd on 2019/10/9.
//

#include "AllDecoder.h"
#include "androidlog.h"
#include "inttypes.h"
extern "C"{
#include <libavutil/time.h>
}

AllDecoder::AllDecoder(MediaSynerCallBack* mediaSynerCallBack) {
    this->synerCallBack = mediaSynerCallBack;
    audioDecoder = new AudioDecoder();
    videoDecoder = new VideoDecoder();
}

float AllDecoder::getDuration() {
    if(fmt_ctx == NULL || fmt_ctx->duration == AV_NOPTS_VALUE){
        return 0;
    }else{
        return (float)fmt_ctx->duration/AV_TIME_BASE;
    }
}

int AllDecoder::getVideoWidth() {
    if(!isValidVideo() || videoDecoder->cc_ctx == NULL){
        return 0;
    }
    return videoDecoder->cc_ctx->width;
}

int AllDecoder::getVideoHeight() {
    if(!isValidVideo() || videoDecoder->cc_ctx == NULL){
        return 0;
    }
    return videoDecoder->cc_ctx->height;
}

void AllDecoder::init(char *url) {
    av_register_all();
    avformat_network_init();
    fmt_ctx = avformat_alloc_context();
    if(fmt_ctx == NULL){
        LOGE("alloc fmt error");
        return;
    }
    int ret = avformat_open_input(&fmt_ctx,url,NULL,NULL);
    if(ret<0){
        LOGE("open input error error: %s",av_err2str(ret));
        return;
    }
    if(avformat_find_stream_info(fmt_ctx,NULL) <0){
        LOGE("find stream info error");
        return;
    }

    for (int i = 0; i < fmt_ctx->nb_streams; ++i) {
        if(fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
        }else
            if(fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i;
        }
    }
    initDecoder(audioDecoder,audio_index);
    initDecoder(videoDecoder,video_index);
}

void AllDecoder::initDecoder(MediaDecoder* decoder,int stream_index){
    if(stream_index == NON_STREAM) return;
    AVCodec* cc = avcodec_find_decoder(fmt_ctx->streams[stream_index]->codecpar->codec_id);
//    AVCodec* cc = avcodec_find_decoder_by_name("h264_mediacodec");
    if(cc == NULL){
        LOGE("find "
             "cc error");
    }
    AVCodecContext * ctx = avcodec_alloc_context3(cc);
    if(ctx == NULL){
        LOGE("alloc ctx error");
        return;
    }
    if(avcodec_parameters_to_context(ctx,
                                     fmt_ctx->streams[stream_index]->codecpar)<0){
        LOGE("parameter error");
        return;
    }
    decoder->stream = fmt_ctx->streams[stream_index];
    decoder->cc_ctx = ctx;
    decoder->cc = cc;
    ctx->thread_count = 8;
    int ret = 0;
    if((ret = avcodec_open2(ctx,0,0))<0){
        LOGE("open avcodec error!!!!%s",av_err2str(ret));
        return;
    }
    decoder->frame_rate = av_guess_frame_rate(fmt_ctx,decoder->stream,NULL);
    decoder->time_base = av_q2d(decoder->stream->time_base);
}

double AllDecoder::caculatDuration(double newPts) {
    double last_duration = newPts - last_video_pts;
    if(isnan(last_duration) || last_duration <0 || last_duration > 3600){
        last_duration = videoDecoder->getFrameDuration();
    }
    return last_duration;
}

double AllDecoder::caculateDelay(double lastDuration) {
    double sync_threshould, diff = 0;
    if(isValidAudio()){
        diff = synerCallBack->getClockDiffByVideoClock();
        sync_threshould = FFMAX(MIN_THRESHOULD,FFMIN(MAX_THRESHOULD,lastDuration));
        if(!isnan(diff) && fabs(diff) <MAX_FRAME_DURATION){
            if(diff < -sync_threshould){
                lastDuration = FFMAX(0,lastDuration+diff);
            }else if (diff >= sync_threshould && diff >MAX_THRESHOULD){
                lastDuration += diff;
            }else if (diff >=sync_threshould){
                lastDuration *= 2;
            }
        }
    }
    return lastDuration;
}

int AllDecoder::getVidepFps() {
    if(fps == -1){
        AVRational fram_rate = av_guess_frame_rate(fmt_ctx,videoDecoder->stream,NULL);
        if(fram_rate.num !=0 && fram_rate.den != 1){
            fps = (int)av_q2d(fram_rate);
        }else if (videoDecoder->stream->avg_frame_rate.num && videoDecoder->stream->avg_frame_rate.den){
            fps = (int)av_q2d(videoDecoder->stream->avg_frame_rate);
        }else if(videoDecoder->stream->r_frame_rate.num && videoDecoder->stream->r_frame_rate.den){
            fps = (int)av_q2d(videoDecoder->stream->r_frame_rate);
        }else{
            fps = (int)1/videoDecoder->time_base;
        }
    }
    return fps;
}

void AllDecoder::checkBufferEnough() {

    int video_size = videoDecoder->Frames.size();
    int audio_size = audioDecoder->getPCMSize();

    if(((video_size <=1 && isValidVideo()) || (audio_size ==0 && isValidAudio()))&& !eof){
        synerCallBack->showLoading();
        buffered = false;
    }else {
        bool isBufferEnough = (video_size >= minbufferduration * getVidepFps()) &&
                              (bufferduration >= minbufferduration);
        if (isBufferEnough || eof) {
            synerCallBack->hideLoading();
            buffered = true;
        }
    }
}

double AllDecoder::fillVideoData(AVFrame **frame) {
    double  sleep_time = 0.0;
    if(completed){
        sleep_time = -1;
        return sleep_time;
    }
    AVFrame* result = NULL;
    checkBufferEnough();
    if(videoDecoder->Frames.size() >0 && buffered){
        result =videoDecoder->getFrame();
        double frame_time = synerCallBack->getFrameTime();
        double pts = result->best_effort_timestamp*videoDecoder->time_base;
        double last_duration = caculatDuration(pts);
        double  delay = caculateDelay(last_duration/speed);
        if(fabs(delay) > 0.1){
            if(delay >0){
                delay = 0.1;
            }else{
                delay = 0;
            }
        }
        double time = av_gettime_relative()/1000000.0;
        if(isnan(frame_time) || time<frame_time){
            frame_time = time;
        }
        synerCallBack->updateVideoClock(pts);
        last_video_pts = pts;
        frame_time += delay;
        if(delay >0 && time - frame_time > MAX_THRESHOULD){
            frame_time = time;
        }
        synerCallBack->updateFrameTime(frame_time);
        sleep_time = frame_time -time;

        if(sleep_time <0){
            LOGE("delay");
            av_frame_free(&result);
            result = NULL;
            sleep_time = 0;
        }
        if(videoDecoder->Frames.size() >0){
            std::unique_lock<std::mutex> lk(videoDecoder->frame_mutex);
            AVFrame* next = videoDecoder->Frames.front();
            double  nextPts = next->best_effort_timestamp*videoDecoder->time_base;
            double duration = caculatDuration(nextPts);
            if(time > duration+frame_time && isValidAudio()){
                videoDecoder->Frames.pop_front();
                last_video_pts = nextPts;
                av_frame_free(&next);
                LOGE(" pop next");
            }
            lk.unlock();
        }
    } else if(eof){
        completed = true;
        LOGE("completed");
    }
    *frame = result;
    return sleep_time;
}


int AllDecoder::fillAudioData(unsigned char *data, int size,int displaiInited) {
    checkBufferEnough();
    if(!displaiInited || completed||seek_req||!buffered){
        memset(data,0,size);
        return 0;
    }
    int needbuffsize = size;

    while (size >0){
        if(currentAudio == NULL){
            if(audioDecoder->getPCMSize() ==0){
                memset(data,0,size);
                return size;
            }
            std::unique_lock<std::mutex> lck(audioDecoder->frame_mutex);
            currentAudio = audioDecoder->getPCM();
            if(currentAudio) {

                currentAudioPos = 0;
                bufferduration -= currentAudio->getDuration();
                currentPts = currentAudio->getPts();
            }
            lck.unlock();
            audioDecoder->fd.notify_all();
        }
        if(currentAudio != NULL){
            byte *samples = currentAudio->getData()+currentAudioPos;
            int dataleft = currentAudio->getSize() - currentAudioPos;
            int num = std::min(dataleft,size);
            memcpy(data,samples,num);
            size -= num;
            data += num;

            if(num < dataleft){
                currentAudioPos += num;
            }else{
                delete currentAudio;
                currentAudio = NULL;
                if(audioDecoder->getPCMSize() ==0 && eof){
                    completed = true;
                }
            }
        }else{

            memset(data,0,size);
            size = 0;
            break;
        }
    }
    return needbuffsize - size;
}

int AllDecoder::handleVideo(AVPacket *avPacket) {
    AVFrame* frame = av_frame_alloc();
    if(avcodec_send_packet(videoDecoder->cc_ctx,avPacket) == 0){
        if(avcodec_receive_frame(videoDecoder->cc_ctx,frame) ==0){
            if(update_seek_after_pts){
                currentPts =frame->pts*videoDecoder->time_base;
                update_seek_after_pts = false;
                LOGE("seek after pts %.5f",currentPts);
            }
            if(!isValidAudio()){
                bufferduration += av_frame_get_pkt_duration(frame)*videoDecoder->time_base;
            }
            if(frame->key_frame)
                synerCallBack->processKeyFrame(frame);
            videoDecoder->putFrame(frame);
        }else{
            av_frame_free(&frame);
        }
    }
    return !isValidAudio() && bufferduration>minbufferduration;
}


int AllDecoder::handleAudio(AVPacket *packet) {
    AVFrame* frame = av_frame_alloc();
    if(avcodec_send_packet(audioDecoder->cc_ctx,packet) == 0){
        while(avcodec_receive_frame(audioDecoder->cc_ctx,frame) == 0){
            AudioFrame* aframe = audioDecoder->coverdata(frame);
            if(aframe == NULL){
                break;
            }
            int ret = synerCallBack->processAudio(aframe);
            if(ret <=0){
                delete aframe;
                break;
            }
            aframe->setSize(ret);
            audioDecoder->putPCM(aframe);
            bufferduration += aframe->getDuration();
            audioDecoder->buff_pts = (float)aframe->getPts();
            if(update_seek_after_pts){
                currentPts =aframe->getPts();
                update_seek_after_pts = false;
                LOGE("seek after pts %.5f",currentPts);
            }
        }
    }
    return bufferduration>minbufferduration;
}

void AllDecoder::decode() {
    AVPacket packet;
    int finish = 0;
    int ret = 0;
    if(seek_req){
        seek();
    }
    while(!finish){
        if(seek_req){
            break;
        }
        ret = av_read_frame(fmt_ctx,&packet);
        if(ret <0){
            LOGE("read frame faild %s",av_err2str(ret));
            av_free_packet(&packet);
            if(ret == AVERROR_EOF){
                eof = true;
            }
            break;
        }
        if(packet.stream_index == video_index){
            finish=handleVideo(&packet);
        }else
            if (packet.stream_index == audio_index){
            finish = handleAudio(&packet);
        }
        av_free_packet(&packet);
    }
    if(eof){
        eof = false;
        if(isValidAudio()) {
            flushAudio();
        }
        if(isValidVideo()) {
            flushVideo();
        }
        eof = true;
    }
}

void AllDecoder::flushVideo() {
    int ret =0;
    avcodec_send_packet(videoDecoder->cc_ctx,NULL);
    while(true){
        AVFrame* frame = av_frame_alloc();
        ret = avcodec_receive_frame(videoDecoder->cc_ctx,frame);
        if(ret == 0){
            videoDecoder->putFrame(frame);
        }else{
            av_frame_free(&frame);
            return;
        }
    }
}


void AllDecoder::flushAudio() {
    int ret = 0;
    avcodec_send_packet(audioDecoder->cc_ctx,NULL);
    AVFrame* frame = av_frame_alloc();
    while(true){
        ret = avcodec_receive_frame(audioDecoder->cc_ctx,frame);
        if(ret == 0){
            AudioFrame* aframe=audioDecoder->coverdata(frame);
            bufferduration+= aframe->getDuration();
            audioDecoder->buff_pts = (float)aframe->getPts();
        }else{
            return;
        }
    }
}


void AllDecoder::seek() {
      if(isValidAudio()){
          int ret = av_seek_frame(fmt_ctx,audio_index,(int64_t)(seek_target/audioDecoder->time_base),AVSEEK_FLAG_BACKWARD);
          if(ret < 0){
              LOGE("seek audio faild");
              seek_req = false;
              return;
          }
      }
      if(isValidVideo()) {
          int ret = av_seek_frame(fmt_ctx, video_index,
                                  (int64_t) (seek_target / videoDecoder->time_base),
                                  AVSEEK_FLAG_BACKWARD);
          if (ret < 0) {
              LOGE("seek video faild");
              seek_req = false;
              return;
          }
      }
    if(isValidVideo()) {
        videoDecoder->clearBuffer();
        avcodec_flush_buffers(videoDecoder->cc_ctx);
    }
    if(isValidAudio()) {
        audioDecoder->clearBuffer();
        avcodec_flush_buffers(audioDecoder->cc_ctx);
    }
    LOGE("seek success");
    seek_req = false;
    update_seek_after_pts = true;
}

void AllDecoder::seekPosition(float position) {
    seek_req = true;
    seek_target = position;
    currentPts = position;
    bufferduration = 0;
}

double AllDecoder::getCurrentPts() const {
    return currentPts;
}

void AllDecoder::setSpeed(double speed) {
    AllDecoder::speed = speed;
}

