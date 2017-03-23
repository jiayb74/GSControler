#include "regrw.h"
#include "ui_regrw.h"
#include "CRegMap.h"
#include "this_config.h"

RegRW::RegRW(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegRW)
{
    ui->setupUi(this);
    ui->addr1->setValidator(new QIntValidator(0, 255, this));
    ui->addr2->setValidator(new QIntValidator(0, 255, this));
    ui->addr3->setValidator(new QIntValidator(0, 255, this));
    ui->writedata->setValidator(new QDoubleValidator(0.0, 666666.0,2, this));
}

RegRW::~RegRW()
{
    Window_show.CloseKeyBoard();
    delete ui;
}

void RegRW::on_write_clicked()
{
    CRegMap m_CRegMap;
    QString addr1str =  ui->addr1->text();
    QString addr2str =  ui->addr2->text();
    QString addr3str =  ui->addr3->text();
    QString datastr =  ui->writedata->text();
    unsigned char  addr1,addr2,addr3,data;
    addr1 = addr1str.toInt();
    addr2 = addr2str.toInt();
    addr3 = addr3str.toInt();
    data  = datastr.toShort();
    m_CRegMap.WriteDoubleByte(addr1,addr2,addr3,data&0xff,(data>>8));
}

void RegRW::on_read_clicked()
{
    CRegMap m_CRegMap;
    QString addr1str =  ui->addr1->text();
    QString addr2str =  ui->addr2->text();
    QString addr3str =  ui->addr3->text();
    unsigned char  addr1,addr2,addr3;
    addr1 = addr1str.toInt();
    addr2 = addr2str.toInt();
    addr3 = addr3str.toInt();
    short data = m_CRegMap.ReadDoubleByte(addr1,addr2,addr3);
    ui->returndata->setText(QString::number(data));

}

void RegRW::on_write_Mul_clicked()
{
    CRegMap m_CRegMap;
    QString addr1str =  ui->addr1->text();
    QString addr2str =  ui->addr2->text();
    QString addr3str =  ui->addr3->text();
    QString datastr =  ui->writedata->text();
    unsigned char  addr1,addr2,addr3;
    float data;
    addr1 = addr1str.toInt();
    addr2 = addr2str.toInt();
    addr3 = addr3str.toInt();
    data  = datastr.toFloat();
    unsigned char * p = (unsigned char *)&data;
    m_CRegMap.WriteFourByte(addr1,addr2,addr3,*p,*(p+1),*(p+2),*(p+3));
}

void RegRW::on_read_Mul_clicked()
{
    CRegMap m_CRegMap;
    QString addr1str =  ui->addr1->text();
    QString addr2str =  ui->addr2->text();
    QString addr3str =  ui->addr3->text();
    unsigned char  addr1,addr2,addr3;
    addr1 = addr1str.toInt();
    addr2 = addr2str.toInt();
    addr3 = addr3str.toInt();
    float data = m_CRegMap.ReadFourByte(addr1,addr2,addr3);
    ui->returndata->setText(QString::number(data));
}

void RegRW::on_reg_return_clicked()
{
    this->close();
}
