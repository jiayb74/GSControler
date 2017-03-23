/********************************************************************************
** Form generated from reading UI file 'can_config.ui'
**
** Created: Thu Dec 4 00:19:00 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAN_CONFIG_H
#define UI_CAN_CONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_can_config
{
public:
    QComboBox *comboBox_2;
    QComboBox *comboBox;
    QLabel *label;
    QLabel *dev;
    QLabel *baud;
    QPushButton *open;
    QComboBox *comboBox_3;
    QLabel *label_2;
    QComboBox *comboBox_4;

    void setupUi(QDialog *can_config)
    {
        if (can_config->objectName().isEmpty())
            can_config->setObjectName(QString::fromUtf8("can_config"));
        can_config->resize(451, 232);
        comboBox_2 = new QComboBox(can_config);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setGeometry(QRect(210, 370, 71, 31));
        comboBox = new QComboBox(can_config);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(210, 330, 71, 31));
        label = new QLabel(can_config);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(160, 340, 66, 17));
        dev = new QLabel(can_config);
        dev->setObjectName(QString::fromUtf8("dev"));
        dev->setGeometry(QRect(60, 40, 91, 17));
        QFont font;
        font.setPointSize(14);
        dev->setFont(font);
        baud = new QLabel(can_config);
        baud->setObjectName(QString::fromUtf8("baud"));
        baud->setGeometry(QRect(60, 90, 81, 17));
        baud->setFont(font);
        open = new QPushButton(can_config);
        open->setObjectName(QString::fromUtf8("open"));
        open->setGeometry(QRect(320, 70, 91, 41));
        comboBox_3 = new QComboBox(can_config);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));
        comboBox_3->setGeometry(QRect(170, 40, 91, 31));
        label_2 = new QLabel(can_config);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(160, 380, 66, 17));
        comboBox_4 = new QComboBox(can_config);
        comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));
        comboBox_4->setGeometry(QRect(170, 90, 91, 31));

        retranslateUi(can_config);

        QMetaObject::connectSlotsByName(can_config);
    } // setupUi

    void retranslateUi(QDialog *can_config)
    {
        can_config->setWindowTitle(QApplication::translate("can_config", "Can Config", 0, QApplication::UnicodeUTF8));
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("can_config", "\346\225\260\346\215\256\345\270\247", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "\350\277\234\347\250\213\345\270\247", 0, QApplication::UnicodeUTF8)
        );
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("can_config", "\346\240\207\345\207\206\345\270\247", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "\346\211\251\345\261\225\345\270\247", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("can_config", "\345\270\247\346\240\274\345\274\217", 0, QApplication::UnicodeUTF8));
        dev->setText(QApplication::translate("can_config", "dev name", 0, QApplication::UnicodeUTF8));
        baud->setText(QApplication::translate("can_config", "baudrate", 0, QApplication::UnicodeUTF8));
        open->setText(QApplication::translate("can_config", "open", 0, QApplication::UnicodeUTF8));
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("can_config", "can0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "can1", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("can_config", "\345\270\247\347\261\273\345\236\213", 0, QApplication::UnicodeUTF8));
        comboBox_4->clear();
        comboBox_4->insertItems(0, QStringList()
         << QApplication::translate("can_config", "1000000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "5000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "100000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "125000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "250000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "500000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("can_config", "800000", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class can_config: public Ui_can_config {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAN_CONFIG_H
