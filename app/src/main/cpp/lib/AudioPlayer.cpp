//
// Created by lucas an on 2022/5/2.
//
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <cstdlib>
#include <mutex>
#include "androidlog.h"
#include "AudioPlayer.h"
#include "AudioDecoder.h"

using namespace std;

#define CHECK_RESULT(res, content) \
if (res != SL_RESULT_SUCCESS) { \
    LOGE("%s fail:%d",content,res) \
    return; \
} \
else{ \
    LOGD("%s success.",content) \
} \


void AudioPlayer::bufferCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    LOGD("bufferCallback")
    auto *player = (AudioPlayer *) context;
//    unique_lock<mutex> uniqueLock(player->audioDecoder->lock);
    if (player->audioDecoder->readOutBufferSize == 0) {
        LOGD("播放结速")
        return;
    }
    (*bufferQueueItf)->Enqueue(bufferQueueItf, player->audioDecoder->outBuffer,
                               player->audioDecoder->readOutBufferSize);
//    player->audioDecoder->continueRead.notify_all();
}

AudioPlayer::AudioPlayer() {
    if (audioDecoder == nullptr) {
        audioDecoder = new AudioDecoder;
    }
}

void AudioPlayer::setFile(const char *filePath) {
    this->filePath = filePath;
    audioDecoder->openAudioFile(filePath);
}

void AudioPlayer::createClient() {
//创建sl对象
    result = slCreateEngine(&engineObj, 0, nullptr, 0, nullptr, nullptr);
    CHECK_RESULT(result, "create engine")
    //实例化
    result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
    CHECK_RESULT(result, "realize engine")
    //获取接口
    result = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
    CHECK_RESULT(result, "get interface")

    //创建混音器
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engine)->CreateOutputMix(engine, &outputMixObj, 1, ids, req);
    CHECK_RESULT(result, "create output mix")
    //实例化混音器
    result = (*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);
    CHECK_RESULT(result, "realize output mix")
    //获取接口
    result = (*outputMixObj)->GetInterface(outputMixObj, SL_IID_ENVIRONMENTALREVERB,
                                           &outputMixEnvReverb);
    CHECK_RESULT(result, "get mix interface")
    //设置混响效果,需要申请权限MODIFY_AUDIO_SETTINGS
    result = (*outputMixEnvReverb)->SetEnvironmentalReverbProperties(outputMixEnvReverb,
                                                                     &outputMixReverbSettings);
    CHECK_RESULT(result, "set env reverb")

    //设置音频参数
    SLDataLocator_AndroidSimpleBufferQueue localBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};//缓冲队列
    //音频参数
    SLDataFormat_PCM dataFormatPcm = {
            SL_DATAFORMAT_PCM,//pcm数据
            2,//声道数
            SL_SAMPLINGRATE_16,//采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,//采样格式 s16le
            SL_PCMSAMPLEFORMAT_FIXED_16,//同上
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//双声道
            SL_BYTEORDER_LITTLEENDIAN//小端
    };
    //数据源
    SLDataSource dataSource = {&localBufferQueue, &dataFormatPcm};
    //输出
    SLDataLocator_OutputMix dataLocatorOutputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObj};
    SLDataSink dataSink = {&dataLocatorOutputMix, nullptr};
    //创建播放器 SLInterfaceID 用于指定接口功能 SLboolean 功能使能
    const SLInterfaceID playerIds[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean playerReq[1] = {SL_BOOLEAN_TRUE};
    result = (*engine)->CreateAudioPlayer(engine, &audioPlayerObj, &dataSource, &dataSink, 1,
                                          playerIds, playerReq);
    CHECK_RESULT(result, "create player")
    result = (*audioPlayerObj)->Realize(audioPlayerObj, SL_BOOLEAN_FALSE);
    CHECK_RESULT(result, "player realize")
    //获取播放器
    result = (*audioPlayerObj)->GetInterface(audioPlayerObj, SL_IID_PLAY, &audioPlayer);
    CHECK_RESULT(result, "get player interface")
    //获取缓冲队列
    result = (*audioPlayerObj)->GetInterface(audioPlayerObj, SL_IID_BUFFERQUEUE, &bufferQueue);
    CHECK_RESULT(result, "get buffer queue interface")
    //注册回调
    result = (*bufferQueue)->RegisterCallback(bufferQueue, AudioPlayer::bufferCallback, this);
    CHECK_RESULT(result, "register callback")
}

void AudioPlayer::play() {
//    if (outBuffer == nullptr) {
//        outBuffer = (char *) malloc(outBufferSize);
//    }

    audioDecoder->startDecode();
    result = (*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING);
    CHECK_RESULT(result, "start player")
    AudioPlayer::bufferCallback(bufferQueue, this);
}

void AudioPlayer::pause() {

}

void AudioPlayer::resume() {

}

void AudioPlayer::stop() {
    result = (*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED);
    CHECK_RESULT(result, "start stop")
}

void AudioPlayer::seek() {

}

void AudioPlayer::release() {

}

AudioPlayer::~AudioPlayer() {
    LOGE("~AudioPlayer")
    if (audioPlayerObj != nullptr) {
        (*audioPlayerObj)->Destroy(audioPlayerObj);
        audioPlayerObj = nullptr;
        audioPlayer = nullptr;
        bufferQueue = nullptr;
    }
//    if (outBuffer != nullptr) {
//        delete outBuffer;
//        outBuffer = nullptr;
//    }
//    if (file != nullptr) {
//        if (file->is_open()) {
//            file->close();
//        }
//        file = nullptr;
//    }

    if (outputMixObj != nullptr) {
        (*outputMixObj)->Destroy(outputMixObj);
        outputMixObj = nullptr;
        outputMixEnvReverb = nullptr;
    }

    if (engineObj != nullptr) {
        (*engineObj)->Destroy(engineObj);
        engineObj = nullptr;
        engine = nullptr;
    }
}
