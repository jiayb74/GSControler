#include "login.h"
#include "ui_login.h"
#include "engineermenu.h"
#include "this_config.h"
extern bool login;

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
}

login::~login()
{
    Window_show.CloseKeyBoard();
    delete ui;
}

void login::on_cancel_clicked()
{
    this->close();
}

void login::on_ok_clicked()
{
    QString pass = ui->passwd->text();
    if(pass == "123")
    {

        Window_show.CloseKeyBoard();
        this->close();
     //   EngineerMode m;
        EngineerMode->exec();
    }
    else
    {
        ui->label->setText("Invaild Password! Please retry:");
    }
}

void login::Introduction(EngineerMenu *_EngineerMode)
{
    EngineerMode = _EngineerMode;
}
