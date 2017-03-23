#include "mylineedit.h"
#include "keyboard.h"
#include "tcpip.h"
#include "ui_tcpip.h"
#include "keyboard.h"
#include "mylineedit.h"
#include "ui_keyboard.h"
#include <Qt>
#include <QtGui>
#include <QPalette>
#include <qtextedit.h>
#include <QLineEdit>
#include "this_config.h"

mylineedit::mylineedit(QWidget * parent):
    QLineEdit(parent)
{
}

//重写mousePressEvent事件，检测事件类型是不是点击另鼠标左键
void mylineedit::mousePressEvent(QMouseEvent *event)
{
    //如果点击另就触发clicked信号
    if(event->button() == Qt::LeftButton)
    {
     //  emit clicked();
      Window_show.ShowKeyBoard(this);
   //    Option p;
       //p.ShowKeyBoard(QWidget,true);


    }
    QLineEdit::mousePressEvent(event);
}
