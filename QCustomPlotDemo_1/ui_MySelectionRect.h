/********************************************************************************
** Form generated from reading UI file 'MySelectionRect.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYSELECTIONRECT_H
#define UI_MYSELECTIONRECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_MySelectionRect
{
public:

    void setupUi(QCustomPlot *MySelectionRect)
    {
        if (MySelectionRect->objectName().isEmpty())
            MySelectionRect->setObjectName(QStringLiteral("MySelectionRect"));
        MySelectionRect->resize(400, 300);

        retranslateUi(MySelectionRect);

        QMetaObject::connectSlotsByName(MySelectionRect);
    } // setupUi

    void retranslateUi(QCustomPlot *MySelectionRect)
    {
        MySelectionRect->setWindowTitle(QApplication::translate("MySelectionRect", "MySelectionRect", 0));
    } // retranslateUi

};

namespace Ui {
    class MySelectionRect: public Ui_MySelectionRect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYSELECTIONRECT_H
