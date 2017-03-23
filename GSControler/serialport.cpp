#include "serialport.h"
#include "ui_serialport.h"

SerialPort::SerialPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPort)
{
    ui->setupUi(this);
}

SerialPort::~SerialPort()
{
    delete ui;
}

void SerialPort::on_ok_clicked()
{
    this->close();
}

void SerialPort::on_cancel_clicked()
{
    this->close();
}
