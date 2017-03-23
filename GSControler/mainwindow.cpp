#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QToolButton>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "login.h"
#include "engineermenu.h"
#include "GsMath.h"
#include "CanDateAnalysis.h"
#include "GSDefine.h"
#include "GSVersion.h"
#include "GsApi.h"
#include "modifypower.h"
#include "RegTable.h"
#include "this_config.h"



using namespace std;
//声明登录标志位变量
extern bool login_flage;

/*
  *函数功能：界面图片的加载
  *输入参数：parent
  *输出参数：NONE
  *返 回 值：MONE
*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);   

    //设置MainWindows窗体背景颜色
    QPalette pal=palette();
    pal.setBrush(QPalette::Window,QBrush(QColor(9, 20,100)));
    setPalette(pal);
    QObject::connect(this->ui->label_Model,
                     SIGNAL(clicked()),
                     this,
                     SLOT(on_GSVersion_clicked()));
    QObject::connect(this->ui->label_power,
                     SIGNAL(clicked()),
                     this,
                     SLOT(ClickModifyPower()));

    int LCD_TYPE = TM070RDH13;
    QDesktopWidget *deskWgt = QApplication::desktop();
        if (deskWgt) {
          int  virtualWidth = deskWgt->width();
          int  virtualHeight = deskWgt->height();
          if((virtualWidth == 800) && (virtualHeight == 480) ){
                LCD_TYPE = TM070RDH13;
          }
          else{
                LCD_TYPE = TM043NDH02;
          }
    }

    if(LCD_TYPE == TM070RDH13)
    {
        ui->label_power->setStyleSheet("font:60pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_Model->setStyleSheet("font:22pt ;background:transparent;color:rgb(100,250,240);");
        ui->label_Width->setStyleSheet("font:22pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_Freq->setStyleSheet("font:22pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_PowerSet->setStyleSheet("font:22pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_StatusCode->setStyleSheet("font:22pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_A->setStyleSheet("font:22pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_W->setStyleSheet("font:22pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_Stage->setStyleSheet("font:18pt ;background:transparent;color:rgb(75,240,125);");

        this->setGeometry(0,0,800,480);
        this->setStyleSheet("background-image: url(:/image/image/70backall.bmp);");
        ui->label_Model->setGeometry(482,62,311,20);
        ui->label_power->setGeometry(360,130,311,141);
        ui->label_A->setGeometry(340,110,21,17);
        ui->label_W->setGeometry(690,230,21,17);
        ui->label_Freq->setGeometry(110,160,161,17);
        ui->label_StatusCode->setGeometry(110,249,161,17);
        ui->label_Width->setGeometry(110,115,161,17);
        ui->label_PowerSet->setGeometry(110,205,161,17);
        ui->label_Stage->setGeometry(50,315,151,20);
        ui->progressBar->setGeometry(210,315,521,20);
         ui->GSVersion->setGeometry(730,10,51,51);
        ui->toolButton_status->setGeometry(93,352,191,81);
        ui->toolButton_config->setGeometry(320,350,191,81);
        ui->toolButton_reset->setGeometry(541,350,191,81);
    }

    else if(LCD_TYPE == TM043NDH02){
        ui->label_Model->setStyleSheet("font:15pt ;background:transparent;color:rgb(100,250,240);");
        ui->label_power->setStyleSheet("font:60pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_A->setStyleSheet("font:18pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_W->setStyleSheet("font:18pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_Freq->setStyleSheet("font:15pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_StatusCode->setStyleSheet("font:15pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_Width->setStyleSheet("font:15pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_PowerSet->setStyleSheet("font:15pt ;background:transparent;color:rgb(75,240,125);");
        ui->label_Stage->setStyleSheet("font:15pt ;background:transparent;color:rgb(75,240,125);");

        this->setGeometry(0,0,481,292);
        this->setStyleSheet("background-image: url(:/image/image/backall.bmp);");
        ui->label_Model->setGeometry(290,30,161,21);
        ui->label_power->setGeometry(200,80,201,91);
        ui->label_A->setGeometry(190,64,19,17);
        ui->label_W->setGeometry(410,140,21,17);
        ui->label_Freq->setGeometry(80,100,91,17);
        ui->label_StatusCode->setGeometry(80,160,91,17);
        ui->label_Width->setGeometry(80,70,81,17);
        ui->label_PowerSet->setGeometry(80,130,91,17);
        ui->label_Stage->setGeometry(40,190,101,20);
        ui->progressBar->setGeometry(150,190,251,16);
        ui->GSVersion->setGeometry(440,10,35,35);
        ui->toolButton_status->setGeometry(40,218,121,41);
        ui->toolButton_config->setGeometry(180,218,121,41);
        ui->toolButton_reset->setGeometry(320,218,121,41);
    }

    ui->label_power->setText("0.00");
    ui->label_A->setVisible(false);
    ui->label_W->setVisible(false);
    ui->label_power->setVisible(false);

    char MachineType[30] = {0};
    char Machine[50] = "Fiber Laser:";
    //machine type
    int RegMap = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_REGMAP,0,GS_INI_HARD_FILE);
    if(RegMap != 0){
        switch(RegMap)
        {
        case GS_REGVERSION_P03H:  strcat(Machine,"Y12-P03H");break;
        case GS_REGVERSION_P07:   strcat(Machine,"Y12-P07");break;
        case GS_REGVERSION_P08:   strcat(Machine,"Y12-P08");break;
        case GS_REGVERSION_P07HM: strcat(Machine,"Y12-P07HM");break;
        }
    }
    else{
        GetPrivateProfileString(GS_INI_SECTION_PRO,GS_INI_KEY_MODEL,"0",MachineType,sizeof(MachineType),GS_INI_FILE);
        strcat(Machine,MachineType);
    }
    ui->label_Model->setText(Machine);
    ui->progressBar->setRange(0,100);
    PreviousPercentage = 0;
    ui->progressBar->setVisible(false);
    ui->label_Stage->setVisible(false);

    EngineerMode = new EngineerMenu;
    customscanner = new CustomScanner;
}

//析构函数
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::MoudelOff(int i)
{
    QMessageBox::about(this,tr("Waring"),tr("LinkStatus disconnect"));
}

void MainWindow::UpDataBar(const QString period, const char value)
{
    ui->progressBar->setVisible(true);
    ui->label_Stage->setVisible(true);
    ui->label_Stage->setText(period);
    ui->progressBar->setValue(value);
}

void MainWindow::Set_ProgressBar(char status)
{
    if(status == SHOWBAR){
        ui->progressBar->setVisible(true);
        ui->label_Stage->setVisible(true);
    }
    else if(status == HIDEBAR){
        ui->progressBar->setVisible(false);
        ui->label_Stage->setVisible(false);
    }
    else
        ui->progressBar->reset();


}
void MainWindow::UI_Laser_State(char state)
{
    if(state == LASER_LASE){
         ui->label_A->setVisible(true);
         ui->label_W->setVisible(true);
         ui->label_power->setVisible(true);
         ui->label_power->setText("0.00");
         ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/lasing.bmp);");
    }
    else if(state == LASER_IDLE){
        ui->label_A->setVisible(false);
        ui->label_W->setVisible(false);
        ui->label_power->setVisible(false);
        ui->label_power->setText("0.00");
        ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/idle.bmp);");
    }
    else if(state == LAUNCH)
        ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/initial.bmp);");
    else if(state == SHUTDOWN)
        ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/shutting.bmp);");
    else if(state == LASER_SCAN)
        ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/calibrate.bmp);");//calibrate
    else if(state == ERROR){
        ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/error.bmp);");
        ui->label_A->setVisible(false);
        ui->label_W->setVisible(false);
        ui->label_power->setVisible(false);
        ui->label_power->setText("0.00");       
    }
}

void MainWindow::UpError()
{
     ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/error.bmp);");
}

void MainWindow::InitGSUI(unsigned char puls, unsigned int freq,bool stats)
{
    char pulsStr[20];
    char freqStr[20];
    sprintf(pulsStr,"%s%d","W:",puls);    //此处应为脉冲宽度---电种子源才有脉冲宽度
    sprintf(freqStr,"%s%d","F:",freq);
    ui->label_Width->setText(pulsStr);
    ui->label_Freq->setText(freqStr);
    if(stats)
         ui->toolButton_status->setStyleSheet("border-image: url(:/image/image/shutting.bmp);");
    //ui->label_powerset->setText("P:0.000");

}

void MainWindow::UI_ShowPower(float presetpower, float power)
{
    ui->label_power->setVisible(true);
    char powerStr[30];
    char PowerValue[10];

    sprintf(powerStr,"%s%.3f","P:",presetpower);
    sprintf(PowerValue,"%.3f",power);

    ui->label_PowerSet->setText(powerStr);
    ui->label_power->setText(PowerValue);
}
/*
void MainWindow::UpdataPlusFreqPower(unsigned char puls, unsigned int freq)
{
    char pulsStr[20];
    char freqStr[20];
    //sprintf(pulsStr,"%s%d","W:",puls);
    sprintf(freqStr,"%s%d","F:",freq);
    //ui->label_Width->setText(pulsStr);
    ui->label_Freq->setText(freqStr);
}
*/
void MainWindow::UpdataPlusFreqPower(float puls,float freq)
{
    char pulsStr[20];
    char freqStr[20];
    sprintf(pulsStr,"%s%.1f","W:",puls);
    sprintf(freqStr,"%s%.1f","F:",freq);
    ui->label_Width->setText(pulsStr);
    ui->label_Freq->setText(freqStr);
}

void MainWindow::on_toolButton_status_clicked()
{
    //需要输入密码
    if(login_flage == true)
    {
         login w;
         w.Introduction(EngineerMode);
         w.exec();
    }
    //无需输入密码
    else
    {
        EngineerMode->exec();
    }
}

void MainWindow::on_toolButton_reset_clicked()
{
    //关闭窗体
    this->close();
    system("reboot");
}

void MainWindow::on_toolButton_config_clicked()
{
    config w;
    //全屏无边框
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.showFullScreen();
    w.exec();
}

void MainWindow::on_GSVersion_clicked()
{
    GSVersion GsVersion;
    GsVersion.show();
    GsVersion.exec();
}

void MainWindow::ClickModifyPower()
{
  ModifyPower mp;
  mp.show();
  mp.exec();
}
