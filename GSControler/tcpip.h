#ifndef TCPIP_H
#define TCPIP_H

#include <QDialog>

namespace Ui {
class tcpip;
}

class tcpip : public QDialog
{
    Q_OBJECT
    
public:
    explicit tcpip(QWidget *parent = 0);
    ~tcpip();
    //配置网口
    void SetNetworkInfo();
    //网管
    void GetGateway();    
private slots:
    //DNS 配置
    void on_dnsbutton_clicked();
    //TCP 配置
    void on_tcp_button_clicked();
    //取消
    void on_cancel_clicked();
    //自动获取IP
    void on_tcp_autoradioButton_clicked();
    //用户自定义静态IP
    void on_tcp_userdefined_clicked();
    //默认DNS配置
    void on_dns_auto_clicked();
    //用户自定义DNS服务器地址
    void on_dns_userdefined_clicked();
    //调用键盘
    void showkey();
    //确定
    void on_ok_clicked();

private:
    Ui::tcpip *ui;
    //是否配置网口
    bool Updata;
    //网关
    QString gateway;
};

#endif // TCPIP_H
