#ifndef SCANNER_H
#define SCANNER_H

#include <QDialog>


namespace Ui {
class Scanner;
}

class Scanner : public QDialog
{
    Q_OBJECT
    
public:
    explicit Scanner(QWidget *parent = 0);
    ~Scanner();
    
private slots:
    void on_sca_return_clicked();
    void on_scan_clicked();
    void on_setpos_clicked();
    void on_getpos_clicked();
    void on_abort_clicked();
    void on_reset_clicked();
    void on_savepos_clicked();
    void curr_pos(unsigned short x,unsigned short y);
    void On_ScanPos_Slot(unsigned short curr_x_pos,unsigned short curr_y_pos,
                         unsigned short tar_x_pos,unsigned short tar_y_pos,unsigned char ScanStatue);
signals:
    void On_Scan_Signal(unsigned short _x_start,unsigned short _x_end,
                   unsigned short _y_start,unsigned short _y_end,
                        unsigned short _step,unsigned char ScanStyle);

    void On_Setpos_Signal(unsigned short _x_tartget,unsigned short _y_target);
    void On_Getpos_Signal();
    void On_Abort_Signal();
    void On_Reset_Signal();
    void On_Savepos_Signal();

private:
    Ui::Scanner *ui;
};

#endif // SCANNER_H
