#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <QDebug>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>constants<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

#define UPTIME_PATH "/proc/uptime"
#define STAT_PATH "/proc/stat"
#define MEMINFO_PATH "/proc/meminfo"
#define HOSTNAME_PATH "/proc/sys/kernel/hostname"
#define CPUINFO_PATH "/proc/cpuinfo"
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60
#define MAX_PROCESS_NUMBER 5000
#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 256
#define DELAY 100000000


int nanosleep(const struct timespec *req, struct timespec *rem);

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>my data types<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

//struct with time for function get_uptime()
struct myTime{
    int hours;
    int minutes;
    int seconds;
};

//struct with data about task for function collect_task_data()
struct taskData{
    char name[MAX_NAME_LEN];
    char state[2];
    pid_t pid;
    double memory;
    double loadCpu;
};

//time given task by pcu
struct taskScheduledTime{
    unsigned long long int utime;
    unsigned long long int stime;
    unsigned long long int cutime;
    unsigned long long int cstime;
};

//cpu times spent in different activities
struct commonCpuTime{
    unsigned long long int user;
    unsigned long long int nice;
    unsigned long long int system;
    unsigned long long int idle;
};

struct sortType{
    int byPid;
    int byName;
    int byCpuLoad;
    int byMemoryLoad;
};

struct searchType{
    int byName;
    int byPid;
};

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>function definition<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

void freeMemory(char** ptr, int strNumber);
char** getMemory(char** ptr, int strNumber, int strSize);
struct myTime getUptime();
int getCommonCpuLoad();
int getCommonMemoryUsage();
void fillNamePidState(const char** pahtToTask, struct taskData* task, int taskNumber);
void fillMemory(const char** pahtToTask, struct taskData* task, int taskNumber);
void fillCpuLoad(const char** pahtToTask, struct taskData* task, int taskNumber);
int getTasksList(struct taskData* taskList);
int signalToTask(pid_t pid, int signal);
unsigned long long int saturatingSub(unsigned long long int a, unsigned long long int b);
float clamp(double cpuLoad, double minCpuLoad, double maxCpuLoad);
void getHostName(char* hostName);
void getUserName(char* userName);
void getCpuModel(char* cpuModel);
unsigned int getMemorySize();
void sortTaskListByPid(struct taskData* taskList, int taskNumber);
void sortTaskListByName(struct taskData* taskList, int taskNumber);
void sortTaskListByMemory(struct taskData* taskList, int taskNumber);
void sortTaskListByCpu(struct taskData* taskList, int taskNumber);
struct taskData* searchByName(struct taskData* taskList, int taskNumber, const char* name);
struct taskData* searchByPid(struct taskData* taskList, int taskNumber, int pid);

#endif // FUNC_H
