#include "ConstantSystemInformation.h"

ConstantSystemInformation::ConstantSystemInformation(){
    this->_setHostName();
    this->_setUserName();
    this->_setCpuModel();
    this->_setMemorySize();
}

ConstantSystemInformation::~ConstantSystemInformation(){
}

void ConstantSystemInformation::_setHostName(){
    char hostName[MAX_NAME_LEN];

    getHostName(hostName);
    this->_hostName = QString(hostName);
}

void ConstantSystemInformation::_setUserName(){
    char userName[MAX_NAME_LEN];

    getUserName(userName);
    this->_userName = QString(userName);
}

void ConstantSystemInformation::_setCpuModel(){
    char cpuModel[MAX_NAME_LEN];

    getCpuModel(cpuModel);
    this->_cpuModel = QString(cpuModel);
}

void ConstantSystemInformation::_setMemorySize(){
    unsigned int memorySize;

    memorySize = getMemorySize();
    this->_memorySize = QString::number(memorySize / 1000000, 10);
    this->_memorySize += " Gb";
}

QString ConstantSystemInformation::_getHostName(){
    return this->_hostName;
}

QString ConstantSystemInformation::_getUserName(){
    return this->_userName;
}

QString ConstantSystemInformation::_getCpuModel(){
    return this->_cpuModel;
}

QString ConstantSystemInformation::_getMemorySize(){
    return this->_memorySize;
}



