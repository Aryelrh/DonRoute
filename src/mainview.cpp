//
// Created by aryel on 11/12/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainView.h" resolved

#include "../header/mainview.h"
#include "ui_mainview.h"


MainView::MainView(QWidget *parent) : QWidget(parent), ui(new Ui::MainView) {
    ui->setupUi(this);
}

MainView::~MainView() {
    delete ui;
}