#include "func.h"

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>finctions for collecting info about all tasks in common<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

struct myTime getUptime(){
    struct myTime time = {-1, -1, -1};
    float uptimeF = 0.0f;
    int uptimeI = 0;
    int good = 0;
    FILE* fileStream = NULL;

    //open file to read uptime
    fileStream = fopen(UPTIME_PATH, "r");
    if(fileStream == NULL){
        return time;
    }

    //read from file and close it
    good = fscanf(fileStream, "%f", &uptimeF);
    fclose(fileStream);

    if(good != 1){
        return time;
    }

    //find time in hours minutes and seconds
    uptimeI = (int)uptimeF;
    time.hours = uptimeI / SECONDS_IN_HOUR;
    uptimeI -= (time.hours * SECONDS_IN_HOUR);
    time.minutes = uptimeI / SECONDS_IN_MINUTE;
    uptimeI -= (time.minutes * SECONDS_IN_MINUTE);
    time.seconds = uptimeI;

    return time;
}

//function get common cpu load
int getCommonCpuLoad(){
    struct commonCpuTime first;
    struct commonCpuTime second;
    struct commonCpuTime delta;
    struct timespec delayTime;
    unsigned long long int total = 1;
    int load = 0;
    int good = 0;
    FILE* fileStream = NULL;

    //init delay struct
    delayTime.tv_sec = 0;
    delayTime.tv_nsec = DELAY;

    //try to open file and read info about cpu
    fileStream = fopen(STAT_PATH, "r");
    if(fileStream == NULL){
        return -1;
    }
    //read data about cpu at t0
    good = fscanf(fileStream, "%*s%llu%llu%llu%llu", &first.user, &first.nice, &first.system, &first.idle);
    if(good != 4) {
        fclose(fileStream);
        return -1;
    }

    //set pointer at the very begining of file
    fseek(fileStream, 0, SEEK_SET);
    nanosleep(&delayTime, NULL);

    //read data about cpu at t1
    good = fscanf(fileStream, "%*s%llu%llu%llu%llu", &second.user, &second.nice, &second.system, &second.idle);
    if(good != 4) {
        fclose(fileStream);
        return -1;
    }

    fclose(fileStream);

    //calculate average load
    delta.user = saturatingSub(second.user, first.user);
    delta.nice = saturatingSub(second.nice, first.nice);
    delta.system = saturatingSub(second.system, first.system);
    delta.idle = saturatingSub(second.idle, first.idle);
    total = delta.user + delta.nice + delta.system + delta.idle;
    load = 100 * (delta.user + delta.nice + delta.system) / total;

    return load;
}

//get data about all memory uasge in system
int getCommonMemoryUsage(){
    long unsigned int totalMemory = 0;
    long unsigned int freeMemory = 0;
    int memoryUsage = 0;
    int good = 0;

    FILE* fileStream = NULL;

    //try to open file
    fileStream = fopen(MEMINFO_PATH, "r");
    if(fileStream == NULL){
        return -1;
    }

    good = fscanf(fileStream, "%*s%lu%*s%*s%lu", &totalMemory, &freeMemory);
    if(good != 2) {
        fclose(fileStream);
        return -1;
    }
    fclose(fileStream);

    memoryUsage =  100 * (totalMemory - freeMemory) / totalMemory;

    return memoryUsage;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>finctions for collecting info about tasks<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

//open and read from file /proc/[pid]/stat information(pid, name, state)+
void fillNamePidState(char** pahtToTask, struct taskData* task, int taskNumber){
    char pathStat[MAX_PATH_LEN + 2];
    FILE* fileStream = NULL;
    int good = 0;

    for(int i = 0; i < taskNumber; i++){
        strncat(pathStat, pahtToTask[i], MAX_PATH_LEN + 1);
        strncat(pathStat, "/stat", 6);

        //open and read info from stat
        fileStream = fopen(pathStat, "r");
        if(fileStream){
            good = fscanf(fileStream, "%d%s%s", &task[i].pid, task[i].name, task[i].state);
            fclose(fileStream);
            if(good != 3){
                task[i].pid = -1;
                strncpy(task[i].name, "read error", 11);
                strncpy(task[i].state, "?", 2);
            }
        }
        fileStream = NULL;
        pathStat[0] = '\0';
    }
}

//open and read two files /proc/[pid]/statm (vmRSS) and /proc/meminfo (totalMem)
void fillMemory(char** pahtToTask, struct taskData* task, int taskNumber){
    char pathStatm[MAX_PATH_LEN + 2];
    long long unsigned int vmRss = 0;
    long long unsigned int totalMemory = 0;
    long long unsigned int freeMemory = 0;
    FILE* fileStreamStatm = NULL;
    FILE* fileStreamMeminfo = NULL;
    int good = 0;

    //open and read total memory
    fileStreamMeminfo = fopen(MEMINFO_PATH, "r");
    if(fileStreamMeminfo){
        good = fscanf(fileStreamMeminfo, "%*s%llu%*s%*s%llu", &totalMemory, &freeMemory);
        fclose(fileStreamMeminfo);
        if(good != 2){
            return;
        }
    }

    //open file /proc/[pid]/statm to read vmRSS of each task and count %mem
    for(int i = 0; i < taskNumber; i++){
        strncat(pathStatm, pahtToTask[i], MAX_PATH_LEN + 1);
        strncat(pathStatm, "/statm", 7);

        //open and read info from statm
        fileStreamStatm = fopen(pathStatm, "r");
        if(fileStreamStatm){
            good = fscanf(fileStreamStatm, "%*d%llu", &vmRss);
            fclose(fileStreamStatm);
            if(good != 1){
                task[i].memory = -1;
                continue;
            }
        }

        task[i].memory = ((double)vmRss / (totalMemory - freeMemory)) * 100.0f;
        fileStreamStatm = NULL;
        pathStatm[0] = '\0';
    }
}

//calculate cpu load by each task in %
void fillCpuLoad(char** pahtToTask, struct taskData* task, int taskNumber){
    struct taskScheduledTime taskTime_t0[taskNumber];
    struct taskScheduledTime taskTime_t1[taskNumber];
    struct taskScheduledTime deltaTask[taskNumber];
    struct commonCpuTime cpuTime_t0;
    struct commonCpuTime cpuTime_t1;
    struct commonCpuTime deltaCpu;
    struct timespec delayTime;
    char pathStat[MAX_PATH_LEN + 2];
    double loadCpu = 0.0f;
    FILE* fileStreamTask = NULL;
    FILE* fileStreamCpu = NULL;
    int good = 0;

    //init delay struct
    delayTime.tv_sec = 0;
    delayTime.tv_nsec = DELAY;

    //open and read cpu times (t0)
    fileStreamCpu = fopen(STAT_PATH, "r");
    if(fileStreamCpu){
        good = fscanf(fileStreamCpu, "%*s%llu%llu%llu%llu", &cpuTime_t0.user, &cpuTime_t0.nice, &cpuTime_t0.system, &cpuTime_t0.idle);
        fclose(fileStreamCpu);
        if(good != 4){
            return;
        }
    }

    //open and read info from /proc/[pid]/stat (utime, stime, cutime, cstime) (t0)
    for(int i = 0; i < taskNumber; i++){
        strncat(pathStat, pahtToTask[i], MAX_PATH_LEN + 1);
        strncat(pathStat, "/stat", 6);

        fileStreamTask = fopen(pathStat, "r");
        if(fileStreamTask){
            good = fscanf(fileStreamTask, "%*d%*s%*s%*d%*d%*d%*d%*d%*u%*u%*u%*u%*u%llu%llu%llu%llu",
                          &taskTime_t0[i].utime, &taskTime_t0[i].stime, &taskTime_t0[i].cutime, &taskTime_t0[i].cstime);
            fclose(fileStreamTask);
            if(good != 4){
                taskTime_t0[i].utime = -1;
                taskTime_t0[i].stime = -1;
                taskTime_t0[i].cutime = -1;
                taskTime_t0[i].cstime = -1;
            }
        }
        fileStreamTask = NULL;
        pathStat[0] = '\0';
    }

    fileStreamCpu = NULL;
    nanosleep(&delayTime, NULL);

    //open and read info from /proc/[pid]/stat (utime, stime, cutime, cstime) (t1)
    for(int i = 0; i < taskNumber; i++){
        strncat(pathStat, pahtToTask[i], MAX_PATH_LEN + 1);
        strncat(pathStat, "/stat", 6);

        fileStreamTask = fopen(pathStat, "r");
        if(fileStreamTask){
            good = fscanf(fileStreamTask, "%*d%*s%*s%*d%*d%*d%*d%*d%*u%*u%*u%*u%*u%llu%llu%llu%llu",
                          &taskTime_t1[i].utime, &taskTime_t1[i].stime, &taskTime_t1[i].cutime, &taskTime_t1[i].cstime);
            fclose(fileStreamTask);
            if(good != 4){
                taskTime_t0[i].utime = -1;
                taskTime_t0[i].stime = -1;
                taskTime_t0[i].cutime = -1;
                taskTime_t0[i].cstime = -1;
            }
        }
        fileStreamTask = NULL;
        pathStat[0] = '\0';
    }

    //open and read cpu times (t1)
    fileStreamCpu = fopen(STAT_PATH, "r");
    if(fileStreamCpu){
        good = fscanf(fileStreamCpu, "%*s%llu%llu%llu%llu", &cpuTime_t1.user, &cpuTime_t1.nice, &cpuTime_t1.system, &cpuTime_t1.idle);
        fclose(fileStreamCpu);
        if(good != 4){
            return;
        }
    }

    //cpu delta of times
    deltaCpu.user = saturatingSub(cpuTime_t1.user, cpuTime_t0.user);
    deltaCpu.nice = saturatingSub(cpuTime_t1.nice, cpuTime_t0.nice);
    deltaCpu.system = saturatingSub(cpuTime_t1.system, cpuTime_t0.system);
    deltaCpu.idle = saturatingSub(cpuTime_t1.idle, cpuTime_t0.idle);

    //calculate cpu load for each task
    for(int i = 0; i < taskNumber; i++){
        deltaTask[i].utime = saturatingSub(taskTime_t1[i].utime, taskTime_t0[i].utime);
        deltaTask[i].stime = saturatingSub(taskTime_t1[i].stime, taskTime_t0[i].stime);
        deltaTask[i].cutime = saturatingSub(taskTime_t1[i].cutime, taskTime_t0[i].cutime);
        deltaTask[i].cstime = saturatingSub(taskTime_t1[i].cstime, taskTime_t0[i].cstime);

        loadCpu = ((double)(deltaTask[i].utime + deltaTask[i].stime) / (deltaCpu.user + deltaCpu.nice + deltaCpu.system + deltaCpu.idle) * 100.0f);
        task[i].loadCpu = clamp(loadCpu, 0.0f, 100.0f);
    }
}

//go by directory /proc and remember names (/proc/[pid]) and then call functions
int getTasksList(struct taskData* taskList){
    int processCounter = 0;
    struct dirent* ep = NULL;
    DIR* dp = NULL;
    int isLetter = 0;
    char** taskPathes = NULL;
    //char taskPathes[MAX_PROCESS_NUMBER][MAX_PATH_LEN];

    taskPathes = getMemory(taskPathes, MAX_PROCESS_NUMBER, MAX_PATH_LEN);
    if(taskPathes == NULL){
        return 0;
    }

    dp = opendir("/proc\0");
    if(dp){
        for(int i = 0; NULL != (ep = readdir(dp)); i++) {
            for(size_t j = 0; j < strlen(ep->d_name); j++){
                if(!((ep->d_name[j] >= '0') && (ep->d_name[j] <='9'))){
                    isLetter = 1;
                    break;
                }
            }
            if(isLetter == 0){
                strncat(taskPathes[processCounter], "/proc/\0", 7);
                strncat(taskPathes[processCounter], ep->d_name, (MAX_PATH_LEN - strlen(ep->d_name)));
                processCounter++;
            }
            else{
                isLetter = 0;
            }
        }
        closedir(dp);
    }

    //call functions to collect info about tasks
    fillNamePidState(taskPathes, taskList, processCounter);
    fillMemory(taskPathes, taskList, processCounter);
    fillCpuLoad(taskPathes, taskList, processCounter);
    freeMemory(taskPathes, MAX_PROCESS_NUMBER);

    return processCounter;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>finctions for counting cpu load<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

unsigned long long int saturatingSub(unsigned long long int a, unsigned long long int b) {
    if (a < b) {
        return 0;
    } else {
        return a - b;
    }
}

float clamp(double cpuLoad, double minCpuLoad, double maxCpuLoad){
    double more = 0.0f;

    more = (cpuLoad > minCpuLoad) ? cpuLoad : minCpuLoad;
    return(cpuLoad > maxCpuLoad) ? maxCpuLoad : more;
}

int signalToTask(pid_t pid, int signal){
    switch(signal){
    case 1: return kill(pid, SIGTERM);
    case 2: return kill(pid, SIGSTOP);
    case 3: return kill(pid, SIGCONT);
    }
    return -1;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>finctions for collecting common info about pc<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

void getHostName(char* hostName){
    FILE* fileStream = NULL;
    int good = 0;

    fileStream = fopen(HOSTNAME_PATH, "r");
    if(fileStream){
        good = fscanf(fileStream, "%s", hostName);
        fclose(fileStream);
        if(good != 1){
            strncpy(hostName, "read error\0", 11);
        }
    }
}

void getUserName(char* userName){
    uid_t uid = 0;
    passwd *pw = NULL;

    uid = geteuid();
    pw = getpwuid(uid);

    strncpy(userName, pw->pw_name, MAX_NAME_LEN);
}

void getCpuModel(char* cpuModel){
    FILE* fileStream = NULL;
    int good = 0;

    fileStream = fopen(CPUINFO_PATH, "r");
    if(fileStream){
        good = fscanf(fileStream, "%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s");
        if(good != 0){
            return;
        }
        cpuModel = fgets(cpuModel, MAX_NAME_LEN, fileStream);
        cpuModel[strlen(cpuModel) - 1] = '\0';
        fclose(fileStream);
    }
}

unsigned int getMemorySize(){
    long unsigned int memory = 0;
    FILE* fileStream = NULL;
    int good = 0;

    fileStream = fopen(MEMINFO_PATH, "r");
    if(fileStream){
        good = fscanf(fileStream, "%*s%lu", &memory);
        fclose(fileStream);
        if(good != 1){
            memory = -1;
        }
    }
    return memory;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>different sort func<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

void sortTaskListByPid(struct taskData* taskList, int taskNumber){
    struct taskData task;

    for(int i = 0; i < taskNumber - 1; i++){
        for(int j = 0; j < taskNumber - i; j++){
            if(taskList[j].pid > taskList[j + 1].pid){
                task = taskList[j];
                taskList[j] = taskList[j + 1];
                taskList[j + 1] = task;
            }
        }
    }
}

void sortTaskListByName(struct taskData* taskList, int taskNumber){
    struct taskData task;

    for(int i = 0; i < taskNumber - 1; i++){
        for(int j = 0; j < taskNumber - i; j++){
            if(strcmp(taskList[j].name, taskList[j + 1].name) > 0){
                task = taskList[j];
                taskList[j] = taskList[j + 1];
                taskList[j + 1] = task;
            }
        }
    }
}

void sortTaskListByMemory(struct taskData* taskList, int taskNumber){
    struct taskData task;

    for(int i = 0; i < taskNumber - 1; i++){
        for(int j = 0; j < taskNumber - i; j++){
            if(taskList[j].memory < taskList[j + 1].memory){
                task = taskList[j];
                taskList[j] = taskList[j + 1];
                taskList[j + 1] = task;
            }
        }
    }
}

void sortTaskListByCpu(struct taskData* taskList, int taskNumber){
    struct taskData task;

    for(int i = 0; i < taskNumber - 1; i++){
        for(int j = 0; j < taskNumber - i; j++){
            if(taskList[j].loadCpu < taskList[j + 1].loadCpu){
                task = taskList[j];
                taskList[j] = taskList[j + 1];
                taskList[j + 1] = task;
            }
        }
    }
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>different search func<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

struct taskData* searchByName(struct taskData* taskList, int taskNumber, const char* name){
    for(int i = 0; i < taskNumber; i++){
        if(0 == strncmp(name, taskList[i].name, strlen(name))){
            return &taskList[i];
        }
    }
    return NULL;
}

struct taskData* searchByPid(struct taskData* taskList, int taskNumber, int pid){
    for(int i = 0; i < taskNumber; i++){
        if(taskList[i].pid == pid){
            return &taskList[i];
        }
    }
    return NULL;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>work with memory<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

void freeMemory(char** ptr, int strNumber){
    for(int i = 0; i < strNumber; i++){
        free(ptr[i]);
    }
    free(ptr);
}

char** getMemory(char** ptr, int strNumber, int strSize){
    ptr = (char**)calloc(strNumber, sizeof(char*));
    if(ptr == NULL){
        return NULL;
    }

    for(int i = 0; i < strNumber; i++){
        ptr[i] = (char*)calloc(strSize, sizeof(char));
        if(ptr[i] == NULL){
           freeMemory(ptr, i);
        }
    }
    return ptr;
}



















