#include "this_config.h"
#include <QMessageBox>


/////Option/////
Option g_OptionData;
Option Window_show;
int User_admin_flag = 0;
Option::Option()
{

}

Option::~Option()
{

}

void Option::ShowKeyBoard(QWidget *parent, bool num) //用于连续点击一个键盘按钮时对话框的显示和隐藏。以及调整显示框位置
{

   if (k_board == NULL)
    {
        k_board = new Keyboard(parent,num);
        QApplication::focusWidget();
        k_board->move(SCREEN_WIDTH - k_board->rect().width(), SCREEN_HEIGHT- k_board->rect().height());
        k_board->show();
        return ;
   }
   else
   {
       if(!k_board->isVisible())
       {
            k_board = new Keyboard(parent,num);
            QApplication::focusWidget();
            k_board->move(SCREEN_WIDTH - k_board->rect().width(), SCREEN_HEIGHT- k_board->rect().height());
            k_board->show();
        return ;
       }
       else
           return ;
    }

   k_board = new Keyboard(parent,num);
   QApplication::focusWidget();
   k_board->move(SCREEN_WIDTH - k_board->rect().width(), SCREEN_HEIGHT- k_board->rect().height());
   k_board->show();

}

void Option::CloseKeyBoard()
{
    delete k_board;
    k_board = NULL;
}

void Option::enablekey()
{
    if(k_board != NULL)
    k_board->close();
}


