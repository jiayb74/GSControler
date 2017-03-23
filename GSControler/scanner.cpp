#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "ui_scanner.h"
#include "this_config.h"

using namespace std;
Scanner::Scanner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Scanner)
{
    ui->setupUi(this);
}

Scanner::~Scanner()
{
    Window_show.CloseKeyBoard();
    delete ui;
}

void Scanner::on_sca_return_clicked()
{
    this->close();
}

void Scanner::on_scan_clicked()
{
    QString s_x_start = ui->start_x->text();
    QString s_x_end   = ui->end_x->text();
    QString s_y_start = ui->start_y->text();
    QString s_y_end   = ui->end_y->text();
    QString s_step    = ui->step->text();
    int index   = ui->scematic->currentIndex();
    unsigned char ScanStyle;
    switch(index){
    case 0:   ScanStyle = 0x03;break;  //sy
    case 1:   ScanStyle = 0x02;break;
    default:ScanStyle = 0x03; break;
    }

    unsigned short x_start = s_x_start.toShort();
    unsigned short x_end = s_x_end.toShort();
    unsigned short y_start = s_y_start.toShort();
    unsigned short y_end = s_y_end.toShort();
    unsigned short step = s_step.toShort();

    emit On_Scan_Signal(x_start,x_end,y_start,y_end,step,ScanStyle);
    QMessageBox::information(this,"GSControl","Scan Starts!");

}

void Scanner::on_setpos_clicked()
{
    QString  x = ui->tar_x->text();
    QString y = ui->tar_y->text();
    unsigned short s_x = x.toShort();
    unsigned short s_y = y.toShort();
    emit On_Setpos_Signal(s_x,s_y);
}

void Scanner::on_getpos_clicked()
{

    emit On_Getpos_Signal();
}

void Scanner::on_abort_clicked()
{
    emit On_Abort_Signal();
}

void Scanner::on_reset_clicked()
{
    emit On_Reset_Signal();
}

void Scanner::on_savepos_clicked()
{

}

void Scanner::curr_pos(unsigned short x, unsigned short y)
{
    QString x_str = QString::number(x);
    QString y_str = QString::number(y);
    ui->pos_x->setText(x_str);
    ui->pos_y->setText(y_str);
}

void Scanner::On_ScanPos_Slot(unsigned short curr_x_pos, unsigned short curr_y_pos, unsigned short tar_x_pos, unsigned short tar_y_pos, unsigned char ScanStatue)
{
    QString curr_x_pos_str = QString::number(curr_x_pos);
    QString curr_y_pos_str = QString::number(curr_y_pos);
    QString tar_x_pos_str = QString::number(tar_x_pos);
    QString tar_y_pos_str = QString::number(tar_y_pos);

    ui->pos_x->setText(curr_x_pos_str);
    ui->pos_y->setText(curr_y_pos_str);
    ui->tar_x->setText(tar_x_pos_str);
    ui->tar_y->setText(tar_y_pos_str);

    if(!ScanStatue){
        QMessageBox::information(this,"GSControl","Scan Completed!");
    }

}
