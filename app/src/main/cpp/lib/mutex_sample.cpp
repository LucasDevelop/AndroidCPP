#include <mutex>
#include "mutex_sample.h"
#include "queue"
#include "androidlog.h"

using namespace std;


[[noreturn]] void Producer::startProduce(MutexSample *sample) {
    for (;;) {
        sample->lock.lock();
        sample->list.push(1);
        sample->lock.unlock();
    }
}

[[noreturn]] void Consumer::startConsume(MutexSample *sample) {
    for (;;) {
        if (sample->list.empty()) {
            sample->lock.lock();
            uint8_t data = sample->list.front();
            LOGD("消费：%d", data)
            sample->lock.unlock();
        } else {

        }
    }
}

void MutexSample::test(Producer *producer, Consumer *consumer) {

}
