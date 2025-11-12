//
// Created by aryel on 11/12/25.
//

#ifndef DONROUTE_MAINVIEW_H
#define DONROUTE_MAINVIEW_H

#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class MainView;
}

QT_END_NAMESPACE

class MainView : public QWidget {
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = nullptr);

    ~MainView() override;

private:
    Ui::MainView *ui;
};


#endif //DONROUTE_MAINVIEW_H