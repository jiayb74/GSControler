#include "mylabel.h"

mylabel::mylabel(QWidget * parent) : QLabel(parent)
{


}

//重写mousePressEvent事件，检测事件类型是不是点击另鼠标左键
void mylabel::mousePressEvent(QMouseEvent *event)
{
    //如果点击另就触发clicked信号
    if(event->button() == Qt::LeftButton)
    {
       emit clicked();
    }
    QLabel::mousePressEvent(event);
}
