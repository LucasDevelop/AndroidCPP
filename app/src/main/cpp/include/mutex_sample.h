#ifndef mutex_sample
#define mutex_sample

#include <mutex>
#include "queue"

#endif
using namespace std;
class MutexSample;

//生产者
class Producer {
public:
    [[noreturn]] void startProduce(MutexSample* sample);
};

//消费者
class Consumer {
public:
    [[noreturn]] void startConsume(MutexSample* sample);
};

class MutexSample {
public:
    queue<uint8_t> list;
    uint8_t max_good = 20;//商品最大容量
    mutex lock;//商品锁
    mutex goodBlock;//商品空了
    mutex goodFull;//商品满了
    void test(Producer* producer,Consumer* consumer);
};

