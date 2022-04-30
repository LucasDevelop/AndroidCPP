#ifndef mutex_sample
#define mutex_sample

#include <mutex>
#include "queue"
#include "common_def.h"

#endif
using namespace std;
class MutexSample;

//生产者
class Producer {
public:
    [[noreturn]] void startProduce(MutexSample* sample,C_STRING tName);
    ~Producer();
};

//消费者
class Consumer {
public:
    [[noreturn]] void startConsume(MutexSample* sample,C_STRING tName);
    ~Consumer();
};

class MutexSample {
public:
    queue<uint32_t> list;
    uint32_t max_good = 20;//商品最大容量
    uint32_t currentData=0;
    mutex lock;//商品锁
    condition_variable notBlock;//商品空了
    condition_variable notFull;//商品满了
    void test();
};

