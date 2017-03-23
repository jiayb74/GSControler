#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>

namespace Ui {
class config;
}

class config : public QDialog
{
    Q_OBJECT
    
public:
    explicit config(QWidget *parent = 0);
    ~config();
    
private slots:
    //串口调试
    void uart();
    //网络配置
    void tcp();
    //CAN通讯
    void on_can_clicked();

private:
    Ui::config *ui;
};

#endif // CONFIG_H
