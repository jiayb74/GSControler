#ifndef MODIFYPOWER_H
#define MODIFYPOWER_H

#include <QDialog>

namespace Ui {
class ModifyPower;
}

class ModifyPower : public QDialog
{
    Q_OBJECT
    
public:
    explicit ModifyPower(QWidget *parent = 0);
    ~ModifyPower();
    
private slots:
    void on_sure_clicked();

    void on_cancel_clicked();

private:
    Ui::ModifyPower *ui;
};

#endif // MODIFYPOWER_H
