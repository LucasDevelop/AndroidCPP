#ifndef audio_sample
#define audio_sample

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <fstream>



using namespace std;

//音频播放器
class AudioPCMPlayer {
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
//缓冲队列
    SLAndroidSimpleBufferQueueItf bufferQueue;
    //文件
    const char *filePath = nullptr;
    ifstream *file = nullptr;
    //文件缓冲
    char *outBuffer = nullptr;
    //文件缓冲大小
    size_t outBufferSize = 16000 * 2 * 2;


    AudioPCMPlayer();

    void setFile(const char *filePath);

    static void bufferCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

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

    ~AudioPCMPlayer();
};
#endif