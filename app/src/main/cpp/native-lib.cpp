#include <jni.h>
#include <string>
#include <thread>
#include "androidlog.h"
#include <unistd.h>
using namespace std;


mutex lock;
int value = 200;
//定义回调函数
void callback(int count,const char* threadName){
    for(int i=count;i>0;i--){
        //互斥锁
        lock.lock();
        LOGD("执行线程%s:%d",threadName,value)
        usleep(1000);
        value--;
        lock.unlock();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lucas_androidcpp_jni_JniThread_createJniThread(JNIEnv *env, jobject thiz) {
    //创建线程
    int count =100;
    LOGD("创建线程")
    thread t = thread(callback, count,"thread1");
    thread t2 = thread(callback, count,"thread2");
    LOGD("创建完成")
    t.join();
    LOGD("------")
    t2.join();
    LOGD("线程执行完成,最终数值:%d",value)
}