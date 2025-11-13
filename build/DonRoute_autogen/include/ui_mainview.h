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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
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
    QTextEdit *textEdit;
    QWidget *tab_3;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lblRoutesMap;
    QVBoxLayout *verticalLayout_4;
    QTextEdit *textEdit_2;
    QComboBox *comboBox;
    QPushButton *btnUseAlgorithm;
    QPushButton *btnReset;
    QWidget *tab_2;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_5;
    QComboBox *combobox_2;
    QPushButton *btnShowTraversal;
    QTextEdit *textEditTraversal;
    QVBoxLayout *verticalLayout_7;
    QPushButton *btnGenerateReport;
    QPushButton *btnSaveData;
    QPushButton *btnLoadData;

    void setupUi(QWidget *MainView)
    {
        if (MainView->objectName().isEmpty())
            MainView->setObjectName("MainView");
        MainView->resize(880, 665);
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
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblInteractiveMap->sizePolicy().hasHeightForWidth());
        lblInteractiveMap->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(lblInteractiveMap);

        textEdit = new QTextEdit(tab);
        textEdit->setObjectName("textEdit");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy1);
        textEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(textEdit);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 1, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        gridLayout_3 = new QGridLayout(tab_3);
        gridLayout_3->setObjectName("gridLayout_3");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lblRoutesMap = new QLabel(tab_3);
        lblRoutesMap->setObjectName("lblRoutesMap");
        sizePolicy.setHeightForWidth(lblRoutesMap->sizePolicy().hasHeightForWidth());
        lblRoutesMap->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(lblRoutesMap);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(40);
        verticalLayout_4->setObjectName("verticalLayout_4");
        textEdit_2 = new QTextEdit(tab_3);
        textEdit_2->setObjectName("textEdit_2");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(textEdit_2->sizePolicy().hasHeightForWidth());
        textEdit_2->setSizePolicy(sizePolicy2);
        textEdit_2->setMaximumSize(QSize(16777215, 500));
        textEdit_2->setReadOnly(true);

        verticalLayout_4->addWidget(textEdit_2);

        comboBox = new QComboBox(tab_3);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");

        verticalLayout_4->addWidget(comboBox);

        btnUseAlgorithm = new QPushButton(tab_3);
        btnUseAlgorithm->setObjectName("btnUseAlgorithm");

        verticalLayout_4->addWidget(btnUseAlgorithm);

        btnReset = new QPushButton(tab_3);
        btnReset->setObjectName("btnReset");

        verticalLayout_4->addWidget(btnReset);


        horizontalLayout_3->addLayout(verticalLayout_4);


        gridLayout_3->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        gridLayout_4 = new QGridLayout(tab_2);
        gridLayout_4->setObjectName("gridLayout_4");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(60);
        verticalLayout_5->setObjectName("verticalLayout_5");
        combobox_2 = new QComboBox(tab_2);
        combobox_2->addItem(QString());
        combobox_2->addItem(QString());
        combobox_2->addItem(QString());
        combobox_2->setObjectName("combobox_2");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(combobox_2->sizePolicy().hasHeightForWidth());
        combobox_2->setSizePolicy(sizePolicy3);

        verticalLayout_5->addWidget(combobox_2);

        btnShowTraversal = new QPushButton(tab_2);
        btnShowTraversal->setObjectName("btnShowTraversal");

        verticalLayout_5->addWidget(btnShowTraversal);

        textEditTraversal = new QTextEdit(tab_2);
        textEditTraversal->setObjectName("textEditTraversal");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(textEditTraversal->sizePolicy().hasHeightForWidth());
        textEditTraversal->setSizePolicy(sizePolicy4);
        textEditTraversal->setReadOnly(true);

        verticalLayout_5->addWidget(textEditTraversal);


        horizontalLayout_4->addLayout(verticalLayout_5);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName("verticalLayout_7");
        btnGenerateReport = new QPushButton(tab_2);
        btnGenerateReport->setObjectName("btnGenerateReport");

        verticalLayout_7->addWidget(btnGenerateReport);

        btnSaveData = new QPushButton(tab_2);
        btnSaveData->setObjectName("btnSaveData");

        verticalLayout_7->addWidget(btnSaveData);

        btnLoadData = new QPushButton(tab_2);
        btnLoadData->setObjectName("btnLoadData");

        verticalLayout_7->addWidget(btnLoadData);


        horizontalLayout_4->addLayout(verticalLayout_7);


        gridLayout_4->addLayout(horizontalLayout_4, 0, 0, 1, 1);

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
        lblInteractiveMap->setText(QString());
        textEdit->setHtml(QCoreApplication::translate("MainView", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Adwaita Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Gu\303\255a de uso:</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Doble click en el mapa para a\303\261adir una estaci\303\263n.</p>\n"
"<p style=\"-qt-par"
                        "agraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Click derecho para renombrar o eliminar estaci\303\263n.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Arrastrar el mouse de una estaci\303\263n a otra para crear una ruta.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainView", "Mapa", nullptr));
        lblRoutesMap->setText(QCoreApplication::translate("MainView", "TextLabel", nullptr));
        textEdit_2->setHtml(QCoreApplication::translate("MainView", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Adwaita Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Gu\303\255a de uso:<br /><br />Presiona una estaci\303\263n para asignar el origen.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Presiona otra estaci\303\263"
                        "n para asignar el destino.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Selecciona el tipo de recorrido abajo.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Presiona el bot\303\263n recorrer para realizar el recorrido.</p></body></html>", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MainView", "Dijkstra", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MainView", "Floyd-Warshall", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("MainView", "Prim", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("MainView", "Kruskal", nullptr));

        btnUseAlgorithm->setText(QCoreApplication::translate("MainView", "Realizar recorrido", nullptr));
        btnReset->setText(QCoreApplication::translate("MainView", "Reiniciar pantalla", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainView", "Consulta de Rutas", nullptr));
        combobox_2->setItemText(0, QCoreApplication::translate("MainView", "Recorrido pre-orden", nullptr));
        combobox_2->setItemText(1, QCoreApplication::translate("MainView", "Recorrido in-orden", nullptr));
        combobox_2->setItemText(2, QCoreApplication::translate("MainView", "Recorrido post-orden", nullptr));

        btnShowTraversal->setText(QCoreApplication::translate("MainView", "Realizar recorrido", nullptr));
        textEditTraversal->setPlaceholderText(QCoreApplication::translate("MainView", "Aqu\303\255 se mostrar\303\241 el recorrido...", nullptr));
        btnGenerateReport->setText(QCoreApplication::translate("MainView", "Generar Reporte", nullptr));
        btnSaveData->setText(QCoreApplication::translate("MainView", "Guardar Datos", nullptr));
        btnLoadData->setText(QCoreApplication::translate("MainView", "Cargar Datos", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainView", "Reportes y Persistencia", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainView: public Ui_MainView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINVIEW_H
