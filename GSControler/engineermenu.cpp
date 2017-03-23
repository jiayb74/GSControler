#include <iostream>
#include "engineermenu.h"
#include "ui_engineermenu.h"
#include "scanner.h"
#include "regrw.h"

using namespace std;
extern bool login_flage;

EngineerMenu::EngineerMenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EngineerMenu)
{
    ui->setupUi(this);
    MotorScanner = new Scanner;
}

EngineerMenu::~EngineerMenu()
{
    delete ui;
}


void EngineerMenu::on_regmap_clicked()
{
    RegRW reg;
    reg.exec();
}

void EngineerMenu::on_scanner_clicked()
{
  //  MotorScanner = new Scanner;
  //  MotorScanner->show();
      MotorScanner->exec();
}

void EngineerMenu::on_shutdown_clicked()
{

    system("echo 0 > /sys/class/backlight/mxs-bl/brightness");
    system("halt");
    exit(0);
}

void EngineerMenu::on_exit_clicked()
{
    login_flage = true;
    this->close();
}

void EngineerMenu::on_cancel_clicked()
{
    login_flage = false;
    this->close();
}
