//
// Created by Jarryd on 2019/9/30.
//
#include "AudioDecoder.h"
#include "androidlog.h"

byte *AudioDecoder::getOutputBuffer(AVFrame *frame) {
    if(frame == NULL){
        return NULL;
    }
    if(datasize <=0){
        datasize =av_samples_get_buffer_size(0,spec->channels,frame->nb_samples,AV_SAMPLE_FMT_S16,1);
    }
    return new byte[datasize];

}

AudioFrame* AudioDecoder::coverdata(AVFrame *frame) {

    uint8_t *out[2] = {0};
    out[0] = getOutputBuffer(frame);
    int samples = 0;
    AudioFrame*aframe = NULL;

    if( (samples =swr_convert(
            swrContext,out,frame->nb_samples,(const uint8_t**)frame->data,
            frame->nb_samples)) >0){
        aframe = new AudioFrame();
        aframe->setPts(frame->pts*time_base);
        aframe->setSize(samples*spec->channels*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));
        aframe->setNbSamples(samples);
        aframe->setData(out[0]);
        double duration = av_frame_get_pkt_duration(frame)*time_base;
        duration = duration == 0?aframe->getSize()/getAudioSpec()->getBytePS():duration;
        aframe->setDuration(duration);
    }else{
        LOGE("swr cover error");
    }
    av_frame_free(&frame);
    return aframe;
}

void AudioDecoder::putPCM(AudioFrame* pcm) {
    std::unique_lock<std::mutex> lk(frame_mutex);
    pcms.push_back(pcm);
    lk.unlock();
    fd.notify_all();
}

AudioFrame* AudioDecoder::getPCM() {
    AudioFrame*data;
    if(pcms.size() >0) {
        data = pcms.front();
        pcms.pop_front();
    }else{
        data = NULL;
    }
    return data;
}

void AudioDecoder::clearBuffer() {
    std::unique_lock<std::mutex> lk(frame_mutex);
    pcms.clear();
    lk.unlock();
}

void AudioDecoder::initAudioSpec() {
    if(cc_ctx == NULL){
        LOGE("audio not inited");
        return;
    }
    spec = new AudioSpec();
    spec->channels = cc_ctx->channels;
    spec->channel_layout = cc_ctx->channel_layout;
    spec->byte_per_sample =av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    spec->sample_rate = cc_ctx->sample_rate;
    if(swrContext == NULL){
        swrContext = swr_alloc_set_opts(
                NULL,av_get_default_channel_layout(spec->channels),
                AV_SAMPLE_FMT_S16,spec->sample_rate,
                av_get_default_channel_layout(cc_ctx->channels),
                cc_ctx->sample_fmt,cc_ctx->sample_rate,0,0
        );
        if(swr_init(swrContext) !=0){
            LOGE("swr init faild");
            return;
        }else{
            LOGE("swr init success");
        }
    }
}

AudioSpec* AudioDecoder::getAudioSpec() {
    if(spec == NULL){
        initAudioSpec();
    }
    return spec;
}

double AudioFrame::getPts()  {
    return pts;
}

void AudioFrame::setPts(double pts) {
    this->pts = pts;
}

byte* AudioFrame::getData() {
    return data;
}

void AudioFrame::setData(byte *data) {
    this->data = data;
}

int AudioFrame::getSize()  {
    return size;
}

void AudioFrame::setSize(int size) {
    this->size = size;
}

double AudioFrame::getDuration()  {
    return duration;
}

void AudioFrame::setDuration(double duration) {
   this->duration = duration;
}

int AudioFrame::getNbSamples() {
    return nb_samples;
}

void AudioFrame::setNbSamples(int nbSamples) {
    nb_samples = nbSamples;
}



AudioFrame::~AudioFrame() {
    pts = 0;
    size = 0;
    duration = 0;
    nb_samples = 0;
    delete data;
}