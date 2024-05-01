#ifndef CONSTANTSYSTEMINFORMATION_H
#define CONSTANTSYSTEMINFORMATION_H

#include "func.h"
#include <QString>

class ConstantSystemInformation{
private:
    QString _hostName;
    QString _userName;
    QString _cpuModel;
    QString _memorySize;

public:
    ConstantSystemInformation();
    ~ConstantSystemInformation();
    QString _getHostName();
    QString _getUserName();
    QString _getCpuModel();
    QString _getMemorySize();
    void _setHostName();
    void _setUserName();
    void _setCpuModel();
    void _setMemorySize();
};

#endif // CONSTANTSYSTEMINFORMATION_H
