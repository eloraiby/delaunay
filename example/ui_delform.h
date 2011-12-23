/********************************************************************************
** Form generated from reading ui file 'delform.ui'
**
** Created: Sat Mar 14 15:27:30 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DELFORM_H
#define UI_DELFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DelForm
{
public:

    void setupUi(QWidget *DelForm)
    {
        if (DelForm->objectName().isEmpty())
            DelForm->setObjectName(QString::fromUtf8("DelForm"));
        DelForm->resize(400, 300);

        retranslateUi(DelForm);

        QMetaObject::connectSlotsByName(DelForm);
    } // setupUi

    void retranslateUi(QWidget *DelForm)
    {
        DelForm->setWindowTitle(QApplication::translate("DelForm", "Form", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DelForm);
    } // retranslateUi

};

namespace Ui {
    class DelForm: public Ui_DelForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DELFORM_H
