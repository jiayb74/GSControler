#include "interrupt.h"

int fd25;
int fd26;

Key_Interrupt * key;

Key_Interrupt::Key_Interrupt()
{
    //打开开中断
    Interrupt_LaserOn();
    //打开关中断
    Interrupt_LaserOff();
    key = this;
}

//抛出开光信号
void Key_Interrupt::Power_Signal(bool status)
{
    emit Power_Switch(status);
}

/*
*函数功能：中断处理函数
*输入参数：信号编号
*输出参数：无
*返 回 值：无
*/
void gpio_signal_fun(int signum)
{
    unsigned char gpio_val;
    read(fd25, &gpio_val, 1);
    if(gpio_val == 25){             //p2.5管脚外部中断 打开激光器
         printf("gpio_val 25: 0x%x\n", gpio_val);
        key->Power_Signal(true);
    }

    gpio_val = 0;

    read(fd26, &gpio_val, 1);
    if(gpio_val == 26){             //p2.6管脚外部中断 关闭激光器
        printf("gpio_val 26: 0x%x\n", gpio_val);
         key->Power_Signal(false);
    }
    printf("interrupt  come\n");
    return ;
}


/*
*函数功能：开关打开中断
*输入参数：无
*输出参数：无
*返 回 值：无
*/
void Key_Interrupt::Interrupt_LaserOn()
{
 //   int fd25;

    int Oflags;
    //信号处理程序
    signal(SIGIO, gpio_signal_fun);

    fd25 = open("/dev/magic-gpiop25", O_RDWR | O_NONBLOCK);
    if (fd25 < 0)
    {
        printf("can't open!\n");
        return ;
    }

    fcntl(fd25, F_SETOWN, getpid());
    Oflags = fcntl(fd25, F_GETFL);
    fcntl(fd25, F_SETFL, Oflags | FASYNC);
 //   return ;
}

/*
*函数功能：开关关闭中断
*输入参数：无
*输出参数：无
*返 回 值：无
*/
void Key_Interrupt::Interrupt_LaserOff()
{
 //   int fd26;

    int Oflags;

    signal(SIGIO, gpio_signal_fun);

    fd26 = open("/dev/magic-gpiop26", O_RDWR | O_NONBLOCK);
    if (fd26 < 0)
    {
        printf("can't open!\n");
        return ;
    }
    fcntl(fd26, F_SETOWN, getpid());
    Oflags = fcntl(fd26, F_GETFL);
    fcntl(fd26, F_SETFL, Oflags | FASYNC);

}
Key_Interrupt::~Key_Interrupt()
{

}


