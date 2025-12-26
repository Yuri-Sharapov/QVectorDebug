/********************************************************************************
** Form generated from reading UI file 'chart_variable.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHART_VARIABLE_H
#define UI_CHART_VARIABLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "src/color_circle_label.h"

QT_BEGIN_NAMESPACE

class Ui_ChartVariable
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *leNAme;
    QHBoxLayout *horizontalLayout;
    QCheckBox *cbEnabled;
    ColorCircleLabel *lblColor;
    QComboBox *cbType;

    void setupUi(QWidget *ChartVariable)
    {
        if (ChartVariable->objectName().isEmpty())
            ChartVariable->setObjectName(QString::fromUtf8("ChartVariable"));
        ChartVariable->resize(200, 80);
        ChartVariable->setMinimumSize(QSize(200, 80));
        verticalLayout = new QVBoxLayout(ChartVariable);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        leNAme = new QLineEdit(ChartVariable);
        leNAme->setObjectName(QString::fromUtf8("leNAme"));

        horizontalLayout_2->addWidget(leNAme);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cbEnabled = new QCheckBox(ChartVariable);
        cbEnabled->setObjectName(QString::fromUtf8("cbEnabled"));
        cbEnabled->setMinimumSize(QSize(30, 30));
        cbEnabled->setMaximumSize(QSize(30, 30));

        horizontalLayout->addWidget(cbEnabled);

        lblColor = new ColorCircleLabel(ChartVariable);
        lblColor->setObjectName(QString::fromUtf8("lblColor"));
        lblColor->setMinimumSize(QSize(30, 30));
        lblColor->setMaximumSize(QSize(30, 30));

        horizontalLayout->addWidget(lblColor);

        cbType = new QComboBox(ChartVariable);
        cbType->addItem(QString());
        cbType->addItem(QString());
        cbType->setObjectName(QString::fromUtf8("cbType"));

        horizontalLayout->addWidget(cbType);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ChartVariable);

        QMetaObject::connectSlotsByName(ChartVariable);
    } // setupUi

    void retranslateUi(QWidget *ChartVariable)
    {
        ChartVariable->setWindowTitle(QCoreApplication::translate("ChartVariable", "Form", nullptr));
        cbEnabled->setText(QString());
        lblColor->setText(QString());
        cbType->setItemText(0, QCoreApplication::translate("ChartVariable", "Signed", nullptr));
        cbType->setItemText(1, QCoreApplication::translate("ChartVariable", "Unsigned", nullptr));

    } // retranslateUi

};

namespace Ui {
    class ChartVariable: public Ui_ChartVariable {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHART_VARIABLE_H
