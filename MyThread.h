#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "DynamicSystemInformation.h"
#include <QThread>

class MyThread : public QThread{
private:
    bool continueRunnig;
    DynamicSystemInformation dynamicSystemInformation;

public:
    MyThread();
    ~MyThread();
    void run();
    void finishRunning();
    DynamicSystemInformation* getDynamicSystemInformation();
};

#endif // MYTHREAD_H
