/*******************************

说明：主要实现通过ARM图形界面接收和发送CAN数据
由于目前采用的实现方法不好，且使用率极低。因此后续再慢慢改进
时间：2015/5/14

*********************************/


#include <QProcess>
#include <QMessageBox>
#include <Qt>
#include <QtGui>
#include <QPalette>
#include <qtextedit.h>
#include <QLineEdit>
#include <QtNetwork>
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/QNetworkInterface>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QFile>
#include "mylineedit.h"
#include "keyboard.h"
#include "tcpip.h"
#include "ui_tcpip.h"
#include "keyboard.h"
#include "ui_keyboard.h"
#include "this_config.h"
#include "mylineedit.h"
#include "can.h"
#include "ui_can.h"
#include "QStandardItemModel"

/*
  *函数功能：CAN通讯构造函数
  *输入参数：parent
  *输出参数：NONE
  *返 回 值：MONE
*/
can::can(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::can)
{
    ui->setupUi(this);

    can_model = new QStandardItemModel;
    //标题
    can_model->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("ID")));
    can_model->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("LENGTH")));
    can_model->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("DATA")));

    ui->frame_data->setModel(can_model);

    ui->frame_data->setColumnWidth(0,130);
    ui->frame_data->setColumnWidth(1,80);
    ui->frame_data->setColumnWidth(2,269);
    //只读
    ui->frame_data->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //默认输入框内容
    ui->id->setText("0000001");
    ui->data->setText("00 01 02 03 04 05 06 07");
    //收发帧数目
    CanRecvDataNUm = 0;
    CanSendDataNUm = 0;
    CanRecvPoc = NULL;
   //默认波特率1000000
    on_baud_currentIndexChanged("1000000");
    //接收使能
    EnableCanRecv();
}

can::~can()
{
    //关闭输入法
    Window_show.CloseKeyBoard();
    //失能CAN接收
    DisEnableCanRecv();
    delete ui;
}


//退出
void can::on_exit_clicked()
{
    this->close();
}

void can::EnableCanRecv()
{
    //启动CAN接收
     CanRecvPoc = new QProcess;
     //有数据时链接到槽函数
     connect(CanRecvPoc, SIGNAL(readyRead()),this, SLOT(recv_can()));
     //启动进程
     CanRecvPoc->start("./canread");
}

//失能接收
void can::DisEnableCanRecv()
{
    if(CanRecvPoc)
    {
        //关闭进程
        CanRecvPoc->close();
        delete CanRecvPoc;
        CanRecvPoc = NULL;
     }
}

//解析CAN接收数据
void can::recv_can()
{
       QString recv;
       //从标准输出读CAN数据
       recv = QString::fromLocal8Bit(CanRecvPoc->readAll().data());
       //分解数据
       QStringList w = recv.split("\n");
       QString id = w.at(0);
       QString length = w.at(1);
       QString data = w.at(2);

       can_model->setItem(CanRecvDataNUm,0,new QStandardItem(id));
       can_model->setItem(CanRecvDataNUm,1,new QStandardItem(length));
       can_model->setItem(CanRecvDataNUm,2,new QStandardItem(data));
        //居中
       can_model->item(CanRecvDataNUm,0)->setTextAlignment(Qt::AlignCenter);
       can_model->item(CanRecvDataNUm,1)->setTextAlignment(Qt::AlignCenter);
       can_model->item(CanRecvDataNUm,2)->setTextAlignment(Qt::AlignCenter);
       //颜色
       can_model->item(CanRecvDataNUm,0)->setForeground(QBrush(QColor(0,249,0)));
       can_model->item(CanRecvDataNUm,1)->setForeground(QBrush(QColor(0,249,0)));
       can_model->item(CanRecvDataNUm,2)->setForeground(QBrush(QColor(0,249,0)));

       CanRecvDataNUm++;

}

//发送数据
void can::on_send_clicked()
{
    //启动CAN发送
    QProcess *CanSendPoc = new QProcess;
    //ID号
    QString sendid = ui->id->text();
    //帧数据
    QString senddata = ui->data->text();
    QString data_sz[8];
    for(int i=0,j=0; i<8; i++,j=j+3){
         data_sz[i] = senddata.mid(j,3);
     }
    //参数列表--帧很ID 帧数据
     QStringList list;
     list<<sendid;
     for(int i=0; i<8; i++){
         list<<data_sz[i];
     }
     CanSendPoc->start("./cansend",list);
     //等待发送结束
     CanSendPoc->waitForFinished();
     CanSendPoc->close();
     delete CanSendPoc;
     CanSendPoc = NULL;

     can_model->setItem(CanRecvDataNUm,0,new QStandardItem(ui->id->text()));
     can_model->setItem(CanRecvDataNUm,1,new QStandardItem("8"));
     can_model->setItem(CanRecvDataNUm,2,new QStandardItem(ui->data->text()));
      //居中
     can_model->item(CanRecvDataNUm,0)->setTextAlignment(Qt::AlignCenter);
     can_model->item(CanRecvDataNUm,1)->setTextAlignment(Qt::AlignCenter);
     can_model->item(CanRecvDataNUm,2)->setTextAlignment(Qt::AlignCenter);

     CanRecvDataNUm++;
}

//判断当前是否接收数据
void can::on_receive_toggled(bool checked)
{
    if(checked == true)
        EnableCanRecv();
    else
        DisEnableCanRecv();
}

//改变CAN波特率
void can::on_baud_currentIndexChanged(QString index)
{
    int flag = 0;
    flag = system("ifconfig can0 down");
    switch (index.toInt())
    {
    case 1000000: flag = system("echo 1000000 > /sys/devices/platform/FlexCAN.0/bitrate");
           break;
    case 500000: flag = system("echo 500000 > /sys/devices/platform/FlexCAN.0/bitrate");
           break;
    }
    flag = system("ifconfig can0 up");
    if(flag == -1){
        QMessageBox::warning(this,"error","CAN波特率设置出错");
    }
}
