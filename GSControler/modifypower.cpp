#include "modifypower.h"
#include "ui_modifypower.h"
#include "SoftSwitch.h"
#include "mylineedit.h"
#include "this_config.h"

extern CSoftSwitch * cs;

ModifyPower::ModifyPower(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyPower)
{
    ui->setupUi(this);
}

ModifyPower::~ModifyPower()
{
    Window_show.CloseKeyBoard();
    delete ui;
}

void ModifyPower::on_sure_clicked()
{
    volatile BYTE byResult = 0;
    volatile BYTE Lstatus = 0;
    QString value = ui->PowerValue->text();
    Lstatus = cs->CheckLaserStage();
    byResult = cs->hotSwap->HandlePowerSwap(value.toDouble(), Lstatus);
    cs->hotSwap->UpdataUI();
    this->close();
}

void ModifyPower::on_cancel_clicked()
{
    this->close();
}
