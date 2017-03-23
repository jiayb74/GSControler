#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H
#include<QLineEdit>
#include<QMouseEvent>

class mylineedit:public QLineEdit
{
    Q_OBJECT
public:
    explicit mylineedit(QWidget * parent = 0);
protected:
    virtual void mousePressEvent(QMouseEvent * event);
signals:
    //重写clicked（）信号
    void clicked();
public slots:
};

#endif // MYLINEEDIT_H
