#include <jni.h>
#include <stdio.h>
#include <thread>
#include "AudioPlayer.h"
#include "AudioDecoder.h"
#include "androidlog.h"

using namespace std;
//
// Created by lucas an on 2022/5/2.
//

AudioPlayer *player= nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPlayer_createAudioPlayer(JNIEnv *env, jobject thiz) {
    if (player== nullptr){
        player = new AudioPlayer;
        player->createClient();
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPlayer_setFile(JNIEnv *env, jobject thiz, jstring file_path) {
    const char *file = env->GetStringUTFChars(file_path, nullptr);
   player->setFile(file);
    LOGD("file:%s",file)
//    auto * audioDecoder = new AudioDecoder;
//    audioDecoder->openAudioFile(file);
//    audioDecoder->startDecode();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPlayer_play(JNIEnv *env, jobject thiz) {
    player->play();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPlayer_pause(JNIEnv *env, jobject thiz) {
   player->pause();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPlayer_resume(JNIEnv *env, jobject thiz) {
    player->resume();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPlayer_stop(JNIEnv *env, jobject thiz) {
    player->stop();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_AudioPlayer_release(JNIEnv *env, jobject thiz) {
    if (player!= nullptr){
        delete player;
        player = nullptr;
    }
}