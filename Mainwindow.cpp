#include "Mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    ui->widget_list->setFont(QFont("Courier New", 11, QFont::Monospace));
    this->setConstantSystemInformation();
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerAlarm()));
    connect(ui->widget_list, SIGNAL(itemSelectionChanged()), this, SLOT(showCurrentItemInStatusBar()));
    timer.setInterval(1000);
    timer.start();
    myThread.start();
}

MainWindow::~MainWindow(){
    myThread.finishRunning();
    myThread.wait();
    delete ui;
}

void MainWindow::setConstantSystemInformation(){
    ui->hostname_lable->setText(this->constantSystemInformation._getHostName());
    ui->user_lable->setText(this->constantSystemInformation._getUserName());
    ui->cpu_lable->setText(this->constantSystemInformation._getCpuModel());
    ui->ram_lable->setText(this->constantSystemInformation._getMemorySize());
}

void MainWindow::timerAlarm(){
    ui->widget_list->clear();
    ui->widget_list->addItems(myThread.getDynamicSystemInformation()->_getTaskList());
    ui->cpu_progress->setValue(myThread.getDynamicSystemInformation()->_getCommonCpuLoad());
    ui->memory_progress->setValue(myThread.getDynamicSystemInformation()->_getCommonMemoryUsage());
    ui->uptime_lable->setText(myThread.getDynamicSystemInformation()->_getUptime());
}

void MainWindow::showCurrentItemInStatusBar(){
    QListWidgetItem* item = ui->widget_list->currentItem();
    ui->statusBar->showMessage(item->text());
}

void MainWindow::on_actionTasks_triggered(){
    ui->page_1->hide();
    ui->page_0->show();
}

void MainWindow::on_actionPerfomance_triggered(){
    ui->page_0->hide();
    ui->page_1->show();
}

void MainWindow::on_actionExit_triggered(){
    QApplication::quit();
}

void MainWindow::on_actionName_triggered(){
    struct sortType sortBy = {0, 1, 0, 0};
    myThread.getDynamicSystemInformation()->_setSortType(sortBy);
}

void MainWindow::on_actionPid_triggered(){
    struct sortType sortBy = {1, 0, 0, 0};
    myThread.getDynamicSystemInformation()->_setSortType(sortBy);
}

void MainWindow::on_actionPercent_CPU_triggered(){
    struct sortType sortBy = {0, 0, 1, 0};
    myThread.getDynamicSystemInformation()->_setSortType(sortBy);
}

void MainWindow::on_actionPercwnt_memory_triggered(){
    struct sortType sortBy = {0, 0, 0, 1};
    myThread.getDynamicSystemInformation()->_setSortType(sortBy);
}

void MainWindow::on_kill_button_clicked(){
    QString buffer;
    int pid;

    buffer = ui->statusBar->currentMessage();
    buffer = buffer.section(' ', 0, 0);
    pid = buffer.toInt(nullptr, 10);
    if(signalToTask(pid, 1) == 0){
        ui->statusBar->showMessage("killed");
    }
    else{
        ui->statusBar->showMessage("you have no right to kill process");
    }
}

void MainWindow::on_continue_button_clicked(){
    QString buffer;
    int pid;

    buffer = ui->statusBar->currentMessage();
    buffer = buffer.section(' ', 0, 0);
    pid = buffer.toInt(nullptr, 10);
    if(signalToTask(pid, 3) == 0){
        ui->statusBar->showMessage("continued");
    }
    else{
        ui->statusBar->showMessage("you have no right to continue process");
    }
}

void MainWindow::on_stop_button_clicked(){
    QString buffer;
    int pid;

    buffer = ui->statusBar->currentMessage();
    buffer = buffer.section(' ', 0, 0);
    pid = buffer.toInt(nullptr, 10);
    if(signalToTask(pid, 2) == 0){
        ui->statusBar->showMessage("stopped");
    }
    else{
        ui->statusBar->showMessage("you have no right to stop process");
    }
}

void MainWindow::on_search_button_clicked(){
    QString buffer;
    bool ok = true;
    struct taskData* foundTask;

    buffer = ui->search_line->text();
    if(myThread.getDynamicSystemInformation()->_getSearchType().byName == 1){
        foundTask = searchByName(myThread.getDynamicSystemInformation()->_getRawTaskList(),
                                 myThread.getDynamicSystemInformation()->_getTaskNumber(),
                                 qPrintable(ui->search_line->text()));
    }
    else{
        foundTask = searchByPid(myThread.getDynamicSystemInformation()->_getRawTaskList(),
                                myThread.getDynamicSystemInformation()->_getTaskNumber(),
                                ui->search_line->text().toInt(&ok, 10));
    }
    if((foundTask != NULL) && ok){
        ui->statusBar->showMessage(QString::asprintf("%-7d %-3s %-6.2f %-6.2f %s", foundTask->pid, foundTask->state, foundTask->memory,
                                                     foundTask->loadCpu, foundTask->name));
    }
    else{
        ui->statusBar->showMessage("could not find task");
    }
}

void MainWindow::on_actionName_2_triggered(){
    struct searchType searchBy = {1, 0};
    myThread.getDynamicSystemInformation()->_setSearchType(searchBy);
}

void MainWindow::on_actionPid_2_triggered(){
    struct searchType searchBy = {0, 1};
    myThread.getDynamicSystemInformation()->_setSearchType(searchBy);
}

void MainWindow::on_actionAbout_QT_triggered(){
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::on_actionAbout_programm_triggered(){
    QMessageBox msgBox;
    msgBox.setText("This cool application help users to controll their system parametrs.");
    msgBox.exec();
}


void MainWindow::on_actionAbout_author_triggered(){
    QMessageBox msgBox;
    msgBox.setText("This application was developed by Stepa Kardash.");
    msgBox.exec();
}


void MainWindow::on_actionhelp_triggered(){
    QMessageBox msgBox;
    msgBox.setText("Unfortunately we can not help you. The world is cruel.");
    msgBox.exec();
}

