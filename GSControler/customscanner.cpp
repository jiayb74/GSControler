#include <QFile>
#include <time.h>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <QTimer>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "customscanner.h"
#include "ui_customscanner.h"

using namespace std;
CustomScanner::CustomScanner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomScanner)
{
    ui->setupUi(this);
}

CustomScanner::~CustomScanner()
{
    delete ui;
}

// style:0 autoalloc style:1 selfcheck
void CustomScanner::ScanStyle(bool style)
{
    fstream file;
    file.open("./doc/Inspector.ini",ios::in);
    if(file){
        ui->label->setText("System is getting ready for self-check...");
        ui->label_2->setVisible(false);
        ui->progressBar->setVisible(false);     
        emit ChangeSwitchState(true);
        this->exec();

    }
    else{
        ui->label_2->setVisible(false);
        ui->progressBar->setVisible(false);
        ui->label->setText("Find no configuration, please return!");
        this->exec();
    }
}

void CustomScanner::StartScan()
{
      emit StartCusScan();
}
void CustomScanner::CloseScanDlg()
{
    this->close();
}


void CustomScanner::UpDateUi(unsigned int _TotalSch, unsigned int _Curr)
{
//  printf("--%d %d\n",_TotalSch,_Curr);
    if((_TotalSch == 0) && (_Curr == 1)){
        ui->label->setVisible(true);
        ui->label_2->setVisible(true);
        ui->progressBar->setVisible(false);
        ui->label->setText("Self-checking complete.");
        ui->label_2->setText("Returning original working point...");

        return ;
    }
    if((_TotalSch == 1) && (_Curr == 0)){
        ui->label->setVisible(false);
        ui->label_2->setVisible(false);
        emit ChangeSwitchState(false);
        this->close();
    }

    char sz1[30];
    char sz2[30];
    sprintf(sz1,"%s%d%s%d","under the test ",_Curr," of ",_TotalSch);
    sprintf(sz2,"%s%d","Prepare for test No.",_Curr);
    QString label_str = sz1;
    QString label1_str = sz2;
    ui->label->setVisible(true);
    ui->label_2->setVisible(true);
    ui->label->setText(label_str);
    ui->label_2->setText(label1_str);
}

void CustomScanner::UpDataBar(float a)
{
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(a*100);
}

void CustomScanner::on_ButtonReturn_clicked()
{
    emit CancelScan();
    this->close();
}
