#include "DynamicSystemInformation.h"

DynamicSystemInformation::DynamicSystemInformation(){
    this->_sortBy = {1, 0, 0, 0};
    this->_searchBy = {0, 1};
}

DynamicSystemInformation::~DynamicSystemInformation(){
}

void DynamicSystemInformation::_setCommonCpuLoad(){
    this->_commonCpuLoad = getCommonCpuLoad();
}

void DynamicSystemInformation::_setCommonMemoryUsage(){
    this->_commonMemoryUsage = getCommonMemoryUsage();
}

void DynamicSystemInformation::_setUptime(){
    myTime uptime;

    uptime = getUptime();
    this->_uptime.clear();

    if(uptime.hours < 10){
        this->_uptime += '0';
        this->_uptime += QString::number(uptime.hours, 10);
    }
    else{
        this->_uptime += QString::number(uptime.hours, 10);
    }
    this->_uptime += ':';

    if(uptime.minutes < 10){
        this->_uptime += '0';
        this->_uptime += QString::number(uptime.minutes, 10);
    }
    else{
        this->_uptime += QString::number(uptime.minutes, 10);
    }
    this->_uptime += ':';

    if(uptime.seconds < 10){
        this->_uptime += '0';
        this->_uptime += QString::number(uptime.seconds, 10);
    }
    else{
        this->_uptime += QString::number(uptime.seconds, 10);
    }
}

void DynamicSystemInformation::_setTaskList(){
    this->_taskNumber = getTasksList(this->_taskList);

    if(this->_sortBy.byPid == 1){
        sortTaskListByPid(this->_taskList, this->_taskNumber);
    }
    else if(this->_sortBy.byName == 1){
        sortTaskListByName(this->_taskList, this->_taskNumber);
    }
    else if(this->_sortBy.byCpuLoad == 1){
        sortTaskListByCpu(this->_taskList, this->_taskNumber);
    }
    else if(this->_sortBy.byMemoryLoad == 1){
        sortTaskListByMemory(this->_taskList, this->_taskNumber);
    }
}

void DynamicSystemInformation::_setSortType(struct sortType sortBy){
    this->_sortBy = sortBy;
}

void DynamicSystemInformation::_setSearchType(struct searchType searchBy){
    this->_searchBy = searchBy;
}

int DynamicSystemInformation::_getCommonCpuLoad(){
    return this->_commonCpuLoad;
}

int DynamicSystemInformation::_getCommonMemoryUsage(){
    return this->_commonMemoryUsage;
}

QString DynamicSystemInformation::_getUptime(){
    return this->_uptime;
}

QStringList DynamicSystemInformation::_getTaskList(){
    QStringList taskList;

    for(int i = 0; i < this->_taskNumber; i++){
        taskList << QString::asprintf("%-7d %-3s %-6.2f %-6.2f %s", this->_taskList[i].pid, this->_taskList[i].state, this->_taskList[i].memory,
                                      this->_taskList[i].loadCpu, this->_taskList[i].name);
    }
    return taskList;
}

struct taskData* DynamicSystemInformation::_getRawTaskList(){
    return this->_taskList;
}

int DynamicSystemInformation::_getTaskNumber(){
    return this->_taskNumber;
}

struct searchType DynamicSystemInformation::_getSearchType(){
    return this->_searchBy;
}






