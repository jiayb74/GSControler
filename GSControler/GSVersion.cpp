#include <QString>
#include "string.h"
#include "GSVersion.h"
#include "ui_GSVersion.h"
#include "GsApi.h"
#include "GSDefine.h"
#include "RegTable.h"
#include "LDKeeper.h"
#include "board.h"

extern LDKeeper * lpLD_temp;
extern Board * boards;

GSVersion::GSVersion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSVersion)
{
    ui->setupUi(this);
    AboutGS();
}

void GSVersion::AboutGS()
{
    CanDataStruct date;
    QString byte0;
    QString byte1;

    char buf[40] = {0};
    //machine type
    GetPrivateProfileString(GS_INI_SECTION_PRO,GS_INI_KEY_MODEL,"0",buf,30,GS_INI_FILE);
    ui->textEdit->append(buf);
    memset(buf,0,sizeof(buf));
    //ARM Version
    ui->textEdit->append(GSVERSION);

    //FPGA Version
    boards[0].LD[0].Read_Set_Reg(0,1,BlockingRead,0,0,0,0,&date);
   // lpLD_temp[SeedRank].Read_Set_Reg(0,1,BlockingRead,0,0,0,0,&date);
    //date.s_data;
    byte0 = QString::number(date.s_data&0xff, 16).toUpper();
    byte1 = QString::number((date.s_data>>8)&0xff, 16).toUpper();
    QString FPGAVersion = "Fversion:";
    FPGAVersion = FPGAVersion + byte1 + "-" + byte0;
    ui->textEdit->append(FPGAVersion);
    //product id
    GetPrivateProfileString(GS_INI_SECTION_PRO,GS_INI_KEY_ID,"0",buf,sizeof(buf),GS_INI_FILE);
    char Product[60] = "ProductID:";
    strcat(Product,buf);
    ui->textEdit->append(Product);
    //about gs
    ui->textEdit->append(GSLTD);

}

GSVersion::~GSVersion()
{
    delete ui;
}

void GSVersion::on_ok_clicked()
{
    this->close();
}
