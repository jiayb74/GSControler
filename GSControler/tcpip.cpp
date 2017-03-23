/********************************
文 件 名：tcpip.c
新建日期：2014/11/15
修改日期：2014/12/5
作    者：王涛
说    明：IP DNS 配置
*********************************/

#include "tcpip.h"
#include "ui_tcpip.h"
#include "keyboard.h"
#include "ui_keyboard.h"
#include <Qt>
#include <QtGui>
#include <QPalette>
#include <qtextedit.h>
#include <QLineEdit>
#include "this_config.h"
#include "mylineedit.h"
#include<QtNetwork>
#include<QtNetwork/qhostaddress.h>
#include<QtNetwork/QNetworkInterface>
#include<QNetworkInterface>
#include<QHostAddress>
#include<QFile>

/*
  *函数功能：IP DNS 配置构造函数
  *输入参数：parent
  *输出参数：NONE
  *返 回 值：MONE
*/
tcpip::tcpip(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tcpip)
{
    ui->setupUi(this);
    //默认显示IP配置，隐藏DNS配置
    ui->dnsframe->hide();
    ui->tcpframe->show();

    ui->ipaddress->setEnabled(false);
    ui->mask->setEnabled(false);
    ui->gateway->setEnabled(false);
    ui->pre_dns->setEnabled(false);
    ui->alt_dns->setEnabled(false);

    //  获取网络IP MASK 地址
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
        //获取所有网络接口的列表
        foreach(QNetworkInterface interface,list)
        {
            QList<QNetworkAddressEntry> entryList = interface.addressEntries();
         //获取IP地址条目列表，每个条目中包含一个IP地址，一个子网掩码和一个广播地址
            foreach(QNetworkAddressEntry entry,entryList)
            {//遍历每一个IP地址条目
                //192开头都IP地址（待完善--实现获取eth0网卡IP地址）
                if(entry.ip().toString().mid(0,3) == "192"){
                    ui->ipaddress->setText(entry.ip().toString());
                    ui->mask->setText(entry.netmask().toString());
                }
            }
        }
        //获取网关
        GetGateway();
}

tcpip::~tcpip()
{
    Window_show.CloseKeyBoard();
    delete ui;
}

/*
  *函数功能：获得网关
  *输入参数：parent
  *输出参数：NONE
  *返 回 值：MONE
*/
void tcpip::GetGateway()
{
    //查看路由
    if(system("route -n | grep 0.0.0.0 > net.txt") == -1){
        QMessageBox::warning(this,"error","获得网关失败");
    }
    QFile f("./net.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(NULL,"error","open error");
    }

    QTextStream txtInput(&f);
    QString lineStr;

    lineStr = txtInput.readLine();
    lineStr = txtInput.readLine();
    ui->gateway->setText(lineStr.mid(10,22));
    gateway = lineStr.mid(10,22);
    f.close();
}

/*
  *函数功能：设置网络IP MASK GATEWAY
  *输入参数：parent
  *输出参数：NONE
  *返 回 值：MONE
*/
void tcpip::SetNetworkInfo()
{
    //获取输入框狂内容
    QString ip = "ifconfig eth0 "+ ui->ipaddress->text();
    QString mask = "ifconfig eth0 "+ ui->mask->text();
    QString gate = "route del default gw "+gateway;
    QString gat = "route add default gw "+ui->gateway->text();
    //设置
    int flag = 0;
    flag = system(ip.toLatin1().data());
    flag = system(mask.toLatin1().data());
    flag = system(gate.toLatin1().data());
    flag = system(gat.toLatin1().data());
    if(flag == -1){
        QMessageBox::warning(this,"error","设置网络错误");
    }
}

/*
  *函数功能：显示键盘
  *输入参数：parent
  *输出参数：NONE
  *返 回 值：MONE
*/
void tcpip::showkey()
{
    //  Window_show.ShowKeyBoard(this);
    //  Option::ShowKeyBoard(this,true);
    //  CloseKeyBoard()
    //  Window_show.CloseKeyBoard();
}

//DNS配置
void tcpip::on_dnsbutton_clicked()
{
    ui->dnsframe->show();
    ui->tcpframe->hide();
}

//TCP配置
void tcpip::on_tcp_button_clicked()
{
    ui->dnsframe->hide();
    ui->tcpframe->show();
}


//取消
void tcpip::on_cancel_clicked()
{
    this->close();
}

//自动配置
void tcpip::on_tcp_autoradioButton_clicked()
{
    ui->ipaddress->setEnabled(false);
    ui->mask->setEnabled(false);
    ui->gateway->setEnabled(false);
    Window_show.enablekey();
}

//手动设置
void tcpip::on_tcp_userdefined_clicked()
{
    ui->ipaddress->setEnabled(true);
    ui->mask->setEnabled(true);
    ui->gateway->setEnabled(true);
}

//DNS自动
void tcpip::on_dns_auto_clicked()
{
    ui->pre_dns->setEnabled(false);
    ui->alt_dns->setEnabled(false);
      Window_show.enablekey();
}

//DNS手动
void tcpip::on_dns_userdefined_clicked()
{
    ui->pre_dns->setEnabled(true);
    ui->alt_dns->setEnabled(true);
}

//确定
void tcpip::on_ok_clicked()
{
    SetNetworkInfo();
    this->close();
}
