//
// Created by aryel on 11/12/25.
//

#ifndef DONROUTE_MAINVIEW_H
#define DONROUTE_MAINVIEW_H

#include <QWidget>
#include <QPixmap>
#include <QPointF>

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainView;
}

QT_END_NAMESPACE

class DataManagement;

class MainView : public QWidget {
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = nullptr);

    ~MainView() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::MainView *ui;
    QPixmap originalPixmap;
    QPixmap scaledPixmapWithStations;
    QPointF lastClickPos;
    DataManagement *dataManagement;
    QSize normalWindowSize;
    bool isInFullscreen;
    bool isInitialized = false;  // Para rastrear si ya se inicializó
    
    // Factores de escala actuales
    float currentScaleX = 1.0f;
    float currentScaleY = 1.0f;
    
    // Métodos privados
    void loadMapImage();
    void resizeLabel();
    void onStationCreated(int id, const QString &name);
    void drawStationsOnMap();
    void updateMapDisplay();
};


#endif //DONROUTE_MAINVIEW_H