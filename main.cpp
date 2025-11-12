#include <QApplication>
#include <QFile>
#include <QPushButton>

#include "header/mainview.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QFile styleFile(":/resources/styles/styles.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    MainView view;

    view.show();
    return app.exec();
}