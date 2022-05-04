//
// Created by lucas an on 2022/5/2.
//

#include "AudioDecoder.h"
#include "androidlog.h"
#include <thread>
#include <unistd.h>
#include <fstream>

extern "C" {
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
}

using namespace std;

#define CHECK_ERR(ret, content) \
if ((ret)!=0){ \
char * errStr = new char[1024]; \
av_strerror(ret,errStr, sizeof(errStr)); \
LOGE("%s error msg:%s",content,errStr) \
return; \
} \


AudioDecoder::AudioDecoder() {
    LOGD("AudioDecoder")
    //初始化
    //注册所有组建
    avdevice_register_all();
}

AudioDecoder::~AudioDecoder() {
    LOGD("~AudioDecoder")
    av_frame_free(&frame);
    av_free(outBuffer);
    swr_free(&swrContext);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);
}

void AudioDecoder::openAudioFile(const char *filePath) {
    //打卡音频文件
    formatContext = avformat_alloc_context();
    CHECK_ERR(avformat_open_input(&formatContext, filePath, nullptr, nullptr),
              "open audio file fail.")
    //获取音频文件信息
    CHECK_ERR(avformat_find_stream_info(formatContext, nullptr), "find stream info fail.")
    LOGD("audio streams nb:%d", formatContext->nb_streams)

    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }
    if (audioStreamIndex == -1) {
        LOGE("can not find audio stream.")
        return;
    }
    codecContext = formatContext->streams[audioStreamIndex]->codec;
    //获取音频参数
    inChannel = codecContext->channel_layout;
    inSampleFormat = codecContext->sample_fmt;
    inSampleRate = codecContext->sample_rate;
}

void AudioDecoder::startDecode() {
    //获取解码器
    avCodec = avcodec_find_decoder(codecContext->codec_id);
    if (avCodec == nullptr) {
        LOGE("not find decoder.")
        return;
    }
    //打开解码器
    CHECK_ERR(avcodec_open2(codecContext, avCodec, nullptr), "open decode devices fail.")
    //配置音频参数
    swrContext = swr_alloc();
    SwrContext *pContext = swr_alloc_set_opts(swrContext,
                                              outChannel,
                                              outSampleFormat,
                                              outSampleRate,
                                              inChannel,
                                              inSampleFormat,
                                              inSampleRate,
                                              0, nullptr
    );
    if (pContext == nullptr) {
        LOGE("设置转码参数失败")
        return;
    }
    CHECK_ERR(swr_init(swrContext), "init 转码器失败")

    //在子线程解码，线程要在堆中创建，不然会被回收。
    std::thread *t = new std::thread(AudioDecoder::run, this);
    LOGD("start decoder..")
}

void AudioDecoder::stopDecode() {

}

void AudioDecoder::run(AudioDecoder *audioDecoder) {
    //解码出来的PCM数据写入到文件
    ofstream file("/data/user/0/com.lucas.androidcpp/files/pcm_cache.pcm");

    //输出声道数量
    int outChanelNb = av_get_channel_layout_nb_channels(audioDecoder->outChannel);
    int outBufferSize = audioDecoder->outSampleRate << 1;
    audioDecoder->outBuffer = (uint8_t *) av_malloc(outBufferSize);
    //压缩数据
    audioDecoder->packet = av_packet_alloc();
    //解压数据
    audioDecoder->frame = av_frame_alloc();
    int got_frame = 0, frame_count = 0, result;
    //一帧一帧的读取音频数据
    while (av_read_frame(audioDecoder->formatContext, audioDecoder->packet) >= 0) {
//        unique_lock<mutex> uniqueLock(audioDecoder->lock);
        if (audioDecoder->packet->stream_index == audioDecoder->audioStreamIndex) {
            //解码
            result = avcodec_decode_audio4(audioDecoder->codecContext, audioDecoder->frame,
                                           &got_frame, audioDecoder->packet);
            if (result < 0) {
                char *errStr = new char[1024];
                av_strerror(result, errStr, sizeof(errStr));
                LOGE("%s error msg:%s", "解码错误!", errStr)
                return;
            }
            if (result == 0) {
                LOGD("解码完成！")
                break;
            }
            if (got_frame > 0) {
                //重采样
                swr_convert(audioDecoder->swrContext, &audioDecoder->outBuffer, outBufferSize,
                            (const uint8_t **) audioDecoder->frame->data,
                            audioDecoder->frame->nb_samples);
                //获取采样大小
                audioDecoder->readOutBufferSize = av_samples_get_buffer_size(nullptr, outChanelNb,
                                                                             audioDecoder->frame->nb_samples,
                                                                             audioDecoder->outSampleFormat,
                                                                             1);
                LOGD("解码次数:%d,大小:%d", frame_count++, audioDecoder->readOutBufferSize)
                file << audioDecoder->outBuffer;
                file.flush();
            }
        }
        av_packet_unref(audioDecoder->packet);
//        usleep(1000 * 100);
//        uniqueLock.unlock();
//        audioDecoder->continueRead.wait(uniqueLock);
    }
    LOGD("解码结束")
    file.close();
}

//写入到文件
void AudioDecoder::writeToFile(const char *filePath) {


}
