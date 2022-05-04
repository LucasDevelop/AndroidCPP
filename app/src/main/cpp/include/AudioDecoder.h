//
// Created by lucas an on 2022/5/2.
//

#ifndef ANDROIDCPP_AUDIODECODER_H
#define ANDROIDCPP_AUDIODECODER_H

#include <mutex>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
}

class AudioDecoder {
public:
    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;
    //解码器
    AVCodec *avCodec = nullptr;
    //音频流下标
    int audioStreamIndex = -1;

    //输入采样格式
    AVSampleFormat inSampleFormat = AV_SAMPLE_FMT_NONE;
    //输入采样率
    int inSampleRate = -1;
    //输入声道布局
    int64_t inChannel = AV_CH_LAYOUT_MONO;

    //输出采样格式
    AVSampleFormat outSampleFormat = AV_SAMPLE_FMT_S16;
    //输出采样率
    int outSampleRate = 16000;
    //输出声道布局
    int64_t outChannel = AV_CH_LAYOUT_STEREO;//双声道

    //输出缓冲
    uint8_t *outBuffer = nullptr;
    //本次获取采样大小
    int readOutBufferSize = 0;
    //线程锁
    std::mutex lock;
    //是否已经消费
    bool isConsume;
    std::condition_variable continueRead;

    //压缩数据
    AVPacket *packet = nullptr;
    //解压数据
    AVFrame *frame = nullptr;
    //转码器
    SwrContext *swrContext = nullptr;

    AudioDecoder();

    ~AudioDecoder();

    void openAudioFile(const char *filePath);

    void startDecode();

    static void run(AudioDecoder *audioDecoder);

    void writeToFile(const char* filePath);

    void stopDecode();
};


#endif //ANDROIDCPP_AUDIODECODER_H
