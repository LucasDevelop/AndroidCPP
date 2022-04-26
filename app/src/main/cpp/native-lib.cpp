#include <jni.h>
#include <string>
#include "androidlog.h"


extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_JniThread_createJniThread(JNIEnv *env, jobject thiz) {
    LOGD("测试日志:%s","哈哈")
}