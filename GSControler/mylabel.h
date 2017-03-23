#ifndef MYLABEL_H
#define MYLABEL_H
#include <QLabel>
#include <QMouseEvent>

class mylabel: public QLabel
 {
  Q_OBJECT
 public:
    mylabel(QWidget * parent = 0);

 protected:
    virtual void mousePressEvent(QMouseEvent *event);
 signals:
    void clicked(void);
 };


#endif // MYLABEL_H
