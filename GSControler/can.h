#ifndef CAN_H
#define CAN_H

#include <QDialog>
#include<QProcess>
#include <QStandardItemModel>

namespace Ui {
class can;
}

class can : public QDialog
{
    Q_OBJECT
    
public:
    explicit can(QWidget *parent = 0);
    ~can();

private slots:

    //退出
    void on_exit_clicked();
    //发送
    void on_send_clicked();
    //接收
    void recv_can();
    //使能接收
    void EnableCanRecv();
    //失能接收
    void DisEnableCanRecv();
    //选择是否接收
    void on_receive_toggled(bool checked);
    //改变波特路
    void on_baud_currentIndexChanged(QString index);

private:
    Ui::can *ui;
    //can接收进程
    QProcess *CanRecvPoc;
    //CAN数据显示表格
    QStandardItemModel *can_model;
    //接收帧总数
    int CanRecvDataNUm;
    //发送帧总素
    int CanSendDataNUm;
};

#endif // CAN_H
