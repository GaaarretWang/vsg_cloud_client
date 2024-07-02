/********************************************************************************
** Form generated from reading UI file 'ARWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ARWIDGET_H
#define UI_ARWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ARWidget
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QGridLayout *layout3;
    QGridLayout *layout1;
    QGridLayout *layout2;
    QGridLayout *layout4;

    void setupUi(QWidget *ARWidget)
    {
        if (ARWidget->objectName().isEmpty())
            ARWidget->setObjectName(QString::fromUtf8("ARWidget"));
        ARWidget->resize(1200, 800);
        gridLayout_2 = new QGridLayout(ARWidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        layout3 = new QGridLayout();
        layout3->setObjectName(QString::fromUtf8("layout3"));
        layout3->setContentsMargins(0, 30, 10, 0);

        gridLayout->addLayout(layout3, 1, 0, 1, 1);

        layout1 = new QGridLayout();
        layout1->setObjectName(QString::fromUtf8("layout1"));
        layout1->setContentsMargins(0, 0, 10, 20);

        gridLayout->addLayout(layout1, 0, 0, 1, 1);

        layout2 = new QGridLayout();
        layout2->setObjectName(QString::fromUtf8("layout2"));
        layout2->setContentsMargins(10, 0, 0, 20);

        gridLayout->addLayout(layout2, 0, 1, 1, 1);

        layout4 = new QGridLayout();
        layout4->setObjectName(QString::fromUtf8("layout4"));
        layout4->setContentsMargins(10, 30, 0, 0);

        gridLayout->addLayout(layout4, 1, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(ARWidget);

        QMetaObject::connectSlotsByName(ARWidget);
    } // setupUi

    void retranslateUi(QWidget *ARWidget)
    {
        ARWidget->setWindowTitle(QApplication::translate("ARWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ARWidget: public Ui_ARWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARWIDGET_H
