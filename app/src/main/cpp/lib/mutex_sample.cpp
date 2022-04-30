#include <mutex>
#include "mutex_sample.h"
#include "thread"
#include "queue"
#include "androidlog.h"
#include "unistd.h"

#define DELAY_MILLISECOND(t) usleep(t*1000);

using namespace std;


[[noreturn]] void Producer::startProduce(MutexSample *sample, C_STRING tName) {
    for (;;) {
        usleep(1);
        unique_lock<mutex> lock(sample->lock);
        if (sample->list.size() >= sample->max_good) {//商品满了，进行等待
            LOGE("%s商品已满", tName)
            sample->notFull.wait(lock);
            LOGE("%s继续生产", tName)
        }
        sample->currentData++;
        sample->list.push(sample->currentData);
        //通知消费者，已经生产出了商品
        DELAY_MILLISECOND(5)//生产耗时
        LOGD("%s生产:%d,剩余商品数：%d", tName, sample->currentData, sample->list.size())
        sample->notBlock.notify_all();
    }
}

Producer::~Producer() {
    LOGE("~Producer")
}

[[noreturn]] void Consumer::startConsume(MutexSample *sample, C_STRING tName) {
    for (;;) {
        usleep(1);
        unique_lock<mutex> lock(sample->lock);
        if (sample->list.empty()) {//商品空了，通知生产,并停止消费
            LOGE("%s商品已空", tName)
            sample->notBlock.wait(lock);
            LOGE("%s继续消费", tName)
        }
        uint32_t data = sample->list.front();
        sample->list.pop();
        //通知消费者已经消费了商品，可以进行生产了
        DELAY_MILLISECOND(1)//消费耗时
        LOGD("%s消费：%d,剩余商品数：%d", tName, data, sample->list.size())
        sample->notFull.notify_all();
    }
}

Consumer::~Consumer() {
    LOGE("~Consumer")
}

void callbackPro(Producer *producer, MutexSample *sample, C_STRING tName) {
    producer->startProduce(sample, tName);
}

void callbackCon(Consumer *consumer, MutexSample *sample, C_STRING tName) {
    consumer->startConsume(sample, tName);
}

void MutexSample::test() {
    Producer producer;
    Consumer consumer;
    thread pt = thread(callbackPro, &producer, this, "pt");
    thread ct = thread(callbackCon, &consumer, this, "ct");
    pt.join();
    ct.join();
}
