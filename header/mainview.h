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
class FileManager;

class MainView : public QWidget {
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = nullptr);

    ~MainView() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnUseAlgorithm_clicked();
    void on_btnReset_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_btnShowTraversal_clicked();
    void on_btnGenerateReport_clicked();
    void on_btnSaveData_clicked();
    void on_btnLoadData_clicked();

private:
    Ui::MainView *ui;
    QPixmap originalPixmap;
    QPixmap scaledPixmapWithStations;
    QPointF lastClickPos;
    DataManagement *dataManagement;
    FileManager *fileManager;
    QSize normalWindowSize;
    bool isInFullscreen;
    bool isInitialized = false;  // Para rastrear si ya se inicializó
    
    // Factores de escala actuales
    float currentScaleX = 1.0f;
    float currentScaleY = 1.0f;
    
    // Variables para drag & drop de conexiones
    bool isDraggingConnection = false;
    int dragSourceStationId = -1;
    QPoint dragCurrentPos;
    
    // Variables para consulta de rutas
    int originStationId = -1;
    int destinationStationId = -1;
    bool isInRoutesTab = false;
    QPixmap routesPixmap;
    float routesScaleX = 1.0f;
    float routesScaleY = 1.0f;
    std::vector<int> algorithmPath;
    float totalPathWeight = 0.0f;
    
    // Métodos privados
    void loadMapImage();
    void resizeLabel();
    void onStationCreated(int id, const QString &name);
    void drawStationsOnMap();
    void updateMapDisplay();
    int getStationAtPosition(const QPointF &pos);
    void showStationContextMenu(int stationId, const QPoint &globalPos);
    void renameStation(int stationId);
    void deleteStation(int stationId);
    QPointF widgetPosToPixmapPos(const QPoint &widgetPos);
    void addConnection(int stationA, int stationB);
    std::pair<int, int> getConnectionAtPosition(const QPointF &pos);
    void showConnectionContextMenu(int stationA, int stationB, const QPoint &globalPos);
    void toggleRouteStatus(int stationA, int stationB);
    
    // Métodos para consulta de rutas
    void drawRoutesMap();
    void updateRoutesDisplay();
    void handleRoutesMapClick(const QPointF &pos);
    int getStationAtPositionRoutes(const QPointF &pos);
    void selectOrigin(int stationId);
    void selectDestination(int stationId);
    void clearSelection();
    void updateButtonStates();
    void runDijkstra();
    void runFloydWarshall();
    void runPrim();
    void runKruskal();
    void drawPathResult();
    void drawTreeResult(const std::vector<std::pair<int, int>>& edges);
    
    // Métodos de guardado/carga
    void saveAllData();
    void loadAllData();
};


#endif //DONROUTE_MAINVIEW_H