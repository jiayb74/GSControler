#ifndef GSVERSION_H
#define GSVERSION_H

#include <QDialog>

namespace Ui {
class GSVersion;
}

class GSVersion : public QDialog
{
    Q_OBJECT
    
public:
    explicit GSVersion(QWidget *parent = 0);
    ~GSVersion();
    void AboutGS();
    
private slots:
    void on_ok_clicked();

private:
    Ui::GSVersion *ui;
};

#endif // GSVERSION_H
