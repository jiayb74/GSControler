#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "engineermenu.h"

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT
    
public:
    explicit login(QWidget *parent = 0);
    ~login();
    void Introduction(EngineerMenu  * _EngineerMode);
    EngineerMenu  * EngineerMode;
    
private slots:
    //取消登录
    void on_cancel_clicked();
    //登录
    void on_ok_clicked();

private:
    Ui::login *ui;
};

#endif // LOGIN_H
