/********************************************************************************
** Form generated from reading UI file 'mainview.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINVIEW_H
#define UI_MAINVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainView
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lblInteractiveMap;
    QVBoxLayout *verticalLayout_3;
    QPushButton *btnAddStation;
    QPushButton *btnDeleteStation;
    QWidget *tab_3;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QGraphicsView *viewRoutesMap;
    QWidget *tab_2;

    void setupUi(QWidget *MainView)
    {
        if (MainView->objectName().isEmpty())
            MainView->setObjectName("MainView");
        MainView->resize(980, 641);
        gridLayout = new QGridLayout(MainView);
        gridLayout->setObjectName("gridLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(-1, -1, 0, -1);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label = new QLabel(MainView);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(19);
        font.setBold(true);
        label->setFont(font);

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(MainView);
        label_2->setObjectName("label_2");

        verticalLayout_2->addWidget(label_2);


        verticalLayout->addLayout(verticalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        tabWidget = new QTabWidget(MainView);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setTabPosition(QTabWidget::TabPosition::South);
        tabWidget->setTabShape(QTabWidget::TabShape::Rounded);
        tab = new QWidget();
        tab->setObjectName("tab");
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName("gridLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        lblInteractiveMap = new QLabel(tab);
        lblInteractiveMap->setObjectName("lblInteractiveMap");
        lblInteractiveMap->setMinimumSize(QSize(400, 250));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblInteractiveMap->sizePolicy().hasHeightForWidth());
        lblInteractiveMap->setSizePolicy(sizePolicy);
        lblInteractiveMap->setAlignment(Qt::AlignCenter);
        lblInteractiveMap->setScaledContents(false);

        horizontalLayout_2->addWidget(lblInteractiveMap);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setSizeConstraint(QLayout::SizeConstraint::SetMinAndMaxSize);
        btnAddStation = new QPushButton(tab);
        btnAddStation->setObjectName("btnAddStation");
        btnAddStation->setMaximumSize(QSize(500, 16777215));

        verticalLayout_3->addWidget(btnAddStation);

        btnDeleteStation = new QPushButton(tab);
        btnDeleteStation->setObjectName("btnDeleteStation");

        verticalLayout_3->addWidget(btnDeleteStation);


        horizontalLayout_2->addLayout(verticalLayout_3);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 1, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        gridLayout_3 = new QGridLayout(tab_3);
        gridLayout_3->setObjectName("gridLayout_3");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        viewRoutesMap = new QGraphicsView(tab_3);
        viewRoutesMap->setObjectName("viewRoutesMap");

        horizontalLayout_3->addWidget(viewRoutesMap);


        gridLayout_3->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        tabWidget->addTab(tab_2, QString());

        horizontalLayout->addWidget(tabWidget);


        verticalLayout->addLayout(horizontalLayout);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(MainView);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainView);
    } // setupUi

    void retranslateUi(QWidget *MainView)
    {
        MainView->setWindowTitle(QCoreApplication::translate("MainView", "DonRoute", nullptr));
        label->setText(QCoreApplication::translate("MainView", "DonRoute", nullptr));
        label_2->setText(QCoreApplication::translate("MainView", "Sistema de rutas de La Mancha", nullptr));
        lblInteractiveMap->setText(QCoreApplication::translate("MainView", "TextLabel", nullptr));
        btnAddStation->setText(QCoreApplication::translate("MainView", "A\303\261adir Estaci\303\263n", nullptr));
        btnDeleteStation->setText(QCoreApplication::translate("MainView", "Borrar Estaci\303\263n", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainView", "Mapa", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainView", "Consulta de Rutas", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainView", "Reportes", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainView: public Ui_MainView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINVIEW_H
