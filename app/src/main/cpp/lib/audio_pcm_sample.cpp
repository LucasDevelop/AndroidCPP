#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <cstdlib>
#include "audio_pcm_sample.h"
#include "androidlog.h"
#include "AudioDecoder.h"

#define CHECK_RESULT(res, content) \
if (res != SL_RESULT_SUCCESS) { \
    LOGE("%s fail:%d",content,res) \
    return; \
} \
else{ \
    LOGD("%s success.",content) \
} \


void AudioPCMPlayer::bufferCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    auto *player = (AudioPCMPlayer *) context;
    if (player->file == nullptr) {
        player->file = new ifstream(player->filePath);
    }
    //重制缓冲
    memset(player->outBuffer, 0, player->outBufferSize);
    player->file->read(player->outBuffer, player->outBufferSize);
    streamsize readSize = player->file->gcount();
    LOGD("play size:%d", readSize)
    if (readSize != player->outBufferSize) {//文件读完了
        return;
    }
    (*bufferQueueItf)->Enqueue(bufferQueueItf, player->outBuffer, readSize);
//    (*(player->bufferQueue))->Enqueue(player->bufferQueue, player->outBuffer, readSize);
}

AudioPCMPlayer::AudioPCMPlayer() {

}

void AudioPCMPlayer::setFile(const char *filePath) {
    this->filePath = filePath;
}

void AudioPCMPlayer::createClient() {
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
    result = (*bufferQueue)->RegisterCallback(bufferQueue, AudioPCMPlayer::bufferCallback, this);
    CHECK_RESULT(result, "register callback")
}

//开始播放
void AudioPCMPlayer::play() {
    if (outBuffer == nullptr) {
        outBuffer = (char *) malloc(outBufferSize);
    }
    result = (*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_PLAYING);
    CHECK_RESULT(result, "start player")
    AudioPCMPlayer::bufferCallback(bufferQueue, this);
}

void AudioPCMPlayer::pause() {

}

void AudioPCMPlayer::resume() {

}

void AudioPCMPlayer::stop() {
    result = (*audioPlayer)->SetPlayState(audioPlayer, SL_PLAYSTATE_STOPPED);
    CHECK_RESULT(result, "start stop")
}

void AudioPCMPlayer::seek() {

}

void AudioPCMPlayer::release() {

}

AudioPCMPlayer::~AudioPCMPlayer() {
    LOGE("AudioPCMPlayeryer")
    if (audioPlayerObj != nullptr) {
        (*audioPlayerObj)->Destroy(audioPlayerObj);
        audioPlayerObj = nullptr;
        audioPlayer = nullptr;
        bufferQueue = nullptr;
    }
    if (outBuffer != nullptr) {
        delete outBuffer;
        outBuffer = nullptr;
    }
    if (file != nullptr) {
        if (file->is_open()) {
            file->close();
        }
        file = nullptr;
    }

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