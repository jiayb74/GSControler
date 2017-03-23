#ifndef CUSTOMSCANNER_H
#define CUSTOMSCANNER_H

#include <QDialog>

namespace Ui {
class CustomScanner;
}

class CustomScanner : public QDialog
{
    Q_OBJECT
    
public:
    explicit CustomScanner(QWidget *parent = 0);
    ~CustomScanner();    
private:
    Ui::CustomScanner *ui;
public slots:
     void ScanStyle(bool style);
     void CloseScanDlg();
private slots:
     void UpDateUi(unsigned int _TotalSch,unsigned int _Curr);
     void UpDataBar(float a);
     void StartScan();
     void on_ButtonReturn_clicked();

signals:
     void StartCusScan();
     void ChangeSwitchState(bool status);
     void CancelScan();
};

#endif // CUSTOMSCANNER_H
