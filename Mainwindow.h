#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ConstantSystemInformation.h"
#include "DynamicSystemInformation.h"
#include "MyThread.h"
#include <QMainWindow>
#include <QTimer>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setConstantSystemInformation();

private slots:
    void timerAlarm();
    void showCurrentItemInStatusBar();
    void on_actionTasks_triggered();
    void on_actionPerfomance_triggered();
    void on_actionExit_triggered();
    void on_actionName_triggered();
    void on_actionPid_triggered();
    void on_actionPercent_CPU_triggered();
    void on_actionPercwnt_memory_triggered();
    void on_kill_button_clicked();
    void on_continue_button_clicked();
    void on_stop_button_clicked();
    void on_search_button_clicked();
    void on_actionName_2_triggered();
    void on_actionPid_2_triggered();
    void on_actionAbout_QT_triggered();
    void on_actionAbout_programm_triggered();
    void on_actionAbout_author_triggered();
    void on_actionhelp_triggered();

private:
    Ui::MainWindow *ui;
    QTimer timer;
    MyThread myThread;
    ConstantSystemInformation constantSystemInformation;
};
#endif // MAINWINDOW_H
