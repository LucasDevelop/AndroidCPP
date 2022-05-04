//
// Created by lucas an on 2022/5/2.
//

#ifndef ANDROIDCPP_AUDIOPLAYER_H
#define ANDROIDCPP_AUDIOPLAYER_H
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "AudioDecoder.h"

//音频播放器
class AudioPlayer {
    //查错
    SLresult result;
    //引擎
    SLObjectItf engineObj = nullptr;
    SLEngineItf engine;
    //混音器
    SLObjectItf outputMixObj = nullptr;
    SLEnvironmentalReverbItf outputMixEnvReverb = nullptr;
    //混响效果
    SLEnvironmentalReverbSettings outputMixReverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //播放器
    SLObjectItf audioPlayerObj = nullptr;
    SLPlayItf audioPlayer;
public:
    //解码器
    AudioDecoder *audioDecoder= nullptr;
    //缓冲队列
    SLAndroidSimpleBufferQueueItf bufferQueue;
    //文件
    const char *filePath = nullptr;

    AudioPlayer();
    static void bufferCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

    void setFile(const char *filePath);

    //创建客户端
    void createClient();

    //开始播放
    void play();

    //暂停
    void pause();

    //恢复
    void resume();

    //停止
    void stop();

    //seek
    void seek();

    //释放资源
    void release();

    ~AudioPlayer();
};


#endif //ANDROIDCPP_AUDIOPLAYER_H
