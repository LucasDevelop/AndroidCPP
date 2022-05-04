#include <jni.h>
#include <string>
#include "androidlog.h"
#include "common_def.h"
#include "audio_pcm_sample.h"
#include <fstream>

AudioPCMPlayer *audioPcmPlayer;

extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPCMPlayer_createAudioPlayer(JNIEnv *env, jobject thiz) {
    if (audioPcmPlayer== nullptr){
        audioPcmPlayer = new AudioPCMPlayer();
    }
    audioPcmPlayer->createClient();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPCMPlayer_play(JNIEnv *env, jobject thiz) {
    audioPcmPlayer->play();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPCMPlayer_pause(JNIEnv *env, jobject thiz) {
    audioPcmPlayer->pause();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPCMPlayer_resume(JNIEnv *env, jobject thiz) {
    audioPcmPlayer->resume();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPCMPlayer_stop(JNIEnv *env, jobject thiz) {
    audioPcmPlayer->stop();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPCMPlayer_release(JNIEnv *env, jobject thiz) {
    audioPcmPlayer->release();
    if (audioPcmPlayer!= nullptr)
        delete audioPcmPlayer;
    audioPcmPlayer = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPCMPlayer_setFile(JNIEnv *env, jobject thiz, jstring file_path) {
    C_STRING file = env->GetStringUTFChars(file_path, nullptr);
    LOGD("文件路径：%s",file)
    audioPcmPlayer->setFile(file);
}