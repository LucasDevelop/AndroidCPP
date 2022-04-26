#ifndef androidlog
#define androidlog
#include <android/log.h>
#endif

#define LOG_TAG "lucas-jni"
#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,FORMAT,##__VA_ARGS__);