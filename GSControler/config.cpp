/********************************
文 件 名：config.c
新建日期：2014/11/15
修改日期：2014/12/5
作    者：王涛
说    明：配置串口
*********************************/

#include "config.h"
#include "ui_config.h"
#include "serialport.h"
#include "tcpip.h"
#include "can.h"

/*
  *函数功能：配置界面
  *输入参数：NONE
  *输出参数：NONE
  *返 回 值：MONE
*/
config::config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::config)
{
    ui->setupUi(this);
    //设置MainWindows窗体背景颜色
    QPalette pal=palette();
    pal.setBrush(QPalette::Window,QBrush(QColor(9, 20,100)));
    setPalette(pal);

    //点击Seriaport按钮推出配置界面
    connect(ui->uart,SIGNAL(clicked()),this,SLOT(uart()));
    //点击tcpip按钮推出配置界面
    connect(ui->tcp,SIGNAL(clicked()),this,SLOT(tcp()));
    //点击OK按钮推出配置界面
    connect(ui->ok,SIGNAL(clicked()),this,SLOT(close()));
    ui->uart->setVisible(false);
    ui->can->setVisible(false);
}

/*
  *函数功能：配置界面
  *输入参数：NONE
  *输出参数：NONE
  *返 回 值：MONE
*/
config::~config()
{
    delete ui;
}

/*
  *函数功能：串口配置界面
  *输入参数：NONE
  *输出参数：NONE
  *返 回 值：MONE
*/
void config::uart()
{
    SerialPort s_port;
    s_port.exec();
}

/*
  *函数功能：网络配置界面
  *输入参数：NONE
  *输出参数：NONE
  *返 回 值：MONE
*/
void config::tcp()
{
    tcpip tcp;
    tcp.exec();
}

/*
  *函数功能：CAN通讯配置界面
  *输入参数：NONE
  *输出参数：NONE
  *返 回 值：MONE
*/
void config::on_can_clicked()
{
   can c;
   c.setWindowFlags(Qt::FramelessWindowHint);
   c.showFullScreen();
   c.exec();
}
