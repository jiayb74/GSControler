#ifndef PORTSTATE_H
#define PORTSTATE_H

#define DEV_PATH "/sys/devices/virtual/gpio/gpio50/value"     //input cheak buttun status P1.18
#define LED_DEV_PATH "/sys/devices/virtual/gpio/gpio77/value" //out control led switch    P2.13
#define POWERSTATUS "/sys/devices/virtual/gpio/gpio62/value" //P24GOOD    P1.30 input
#define LOOPUP "/sys/devices/virtual/gpio/gpio63/value" //loopup   P1.31    output

#include<QThread>

class PortState:public QThread
{
    Q_OBJECT
public:
    PortState();
    ~PortState();
    void run();
    unsigned char GetSwitchStatus();
    void InitP24V();

private:
    int fd;
    int led_fd;
    int P24V_fd;
    int Loop_fd;
    char value;
    char p24value;
    char lebvalue;
    char loopvalue;
    char PreviousValue;
    char P24VPreviousValue;
    bool SwitchFlag;
    int  discount;
    bool isDisPower;
public slots:
    void cheakport();
    void cheakP24port();
    void DisPower();
    void Change_Led_state(bool state);
    void Protect(bool state);
signals:
    void SwitchState(bool state);

};

#endif // PORTSTATE_H
