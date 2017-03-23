#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "scanner.h"
#include "engineermenu.h"
#include "customscanner.h"

using namespace std;
class EngineerMenu;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    EngineerMenu  * EngineerMode;
    CustomScanner * customscanner;
    
public slots:
    void UpDataBar(const QString period,const char value);
    void Set_ProgressBar(char status);
    void MoudelOff(int i);
    void UI_Laser_State(char state);
    void UI_ShowPower(float presetpower,float power);
    //void UpdataPlusFreqPower(unsigned char puls,unsigned int freq);
    void UpdataPlusFreqPower(float puls,float freq);
    void InitGSUI(unsigned char puls,unsigned int freq, bool stats);
    void UpError();


private slots:
    void on_toolButton_reset_clicked();

    void on_toolButton_status_clicked();

    void on_toolButton_config_clicked();

    void on_GSVersion_clicked();

    void ClickModifyPower();

private:
    Ui::MainWindow *ui;
    char PreviousPercentage;
};

#endif // MAINWINDOW_H



