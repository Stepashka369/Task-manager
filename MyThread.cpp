#include "MyThread.h"

MyThread::MyThread(){
    this->continueRunnig = true;
}

MyThread::~MyThread(){
}

void MyThread::run(){
    while(this->continueRunnig){
        dynamicSystemInformation._setTaskList();
        dynamicSystemInformation._setUptime();
        dynamicSystemInformation._setCommonMemoryUsage();
        dynamicSystemInformation._setCommonCpuLoad();
    }
}

void MyThread::finishRunning(){
    this->continueRunnig = false;
}

DynamicSystemInformation* MyThread::getDynamicSystemInformation(){
    return &this->dynamicSystemInformation;
}
