#ifndef DYNAMICSYSTEMINFORMATION_H
#define DYNAMICSYSTEMINFORMATION_H

#include "func.h"
#include <QString>
#include <QStringList>

class DynamicSystemInformation{
private:
    int _commonCpuLoad;
    int _commonMemoryUsage;
    QString _uptime;
    struct sortType _sortBy;
    struct searchType _searchBy;
    struct taskData _taskList[MAX_PROCESS_NUMBER];
    int _taskNumber;

public:
    DynamicSystemInformation();
    ~DynamicSystemInformation();
    void _setCommonCpuLoad();
    void _setCommonMemoryUsage();
    void _setUptime();
    void _setTaskList();
    void _setSortType(struct sortType sortBy);
    void _setSearchType(struct searchType searchBy);
    int _getCommonCpuLoad();
    int _getCommonMemoryUsage();
    QString _getUptime();
    QStringList _getTaskList();
    struct taskData* _getRawTaskList();
    int _getTaskNumber();
    struct searchType _getSearchType();
};

#endif // DYNAMICSYSTEMINFORMATION_H
