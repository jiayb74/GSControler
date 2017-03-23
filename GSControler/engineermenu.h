#ifndef ENGINEERMENU_H
#define ENGINEERMENU_H

#include <QDialog>
#include <iostream>
#include "mainwindow.h"
#include "scanner.h"

using namespace std;

class MainWindow;
namespace Ui {
class EngineerMenu;
}

class EngineerMenu : public QDialog
{
    Q_OBJECT    
public:
    explicit EngineerMenu(QWidget *parent = 0);
    ~EngineerMenu();
    Scanner * MotorScanner;
private slots:
    void on_regmap_clicked();
    void on_scanner_clicked();
    //关闭程序
    void on_shutdown_clicked();
    //完全退出
    void on_exit_clicked();
    //退出
    void on_cancel_clicked();
//signals:
private:
    Ui::EngineerMenu *ui;
};

#endif // ENGINEERMENU_H
