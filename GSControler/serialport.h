#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QDialog>

namespace Ui {
class SerialPort;
}

class SerialPort : public QDialog
{
    Q_OBJECT    
public:
    explicit SerialPort(QWidget *parent = 0);
    ~SerialPort();    
private slots:
    void on_ok_clicked();
    void on_cancel_clicked();
private:
    Ui::SerialPort *ui;
};

#endif // SERIALPORT_H
