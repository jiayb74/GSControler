#ifndef REGRW_H
#define REGRW_H

#include <QDialog>

namespace Ui {
class RegRW;
}

class RegRW : public QDialog
{
    Q_OBJECT
    
public:
    explicit RegRW(QWidget *parent = 0);
    ~RegRW();
    
private slots:
    void on_write_clicked();

    void on_read_clicked();

    void on_write_Mul_clicked();

    void on_read_Mul_clicked();

    void on_reg_return_clicked();

private:
    Ui::RegRW *ui;
};

#endif // REGRW_H
