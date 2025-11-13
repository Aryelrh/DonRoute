//
// Created by aryel on 11/12/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainView.h" resolved

#include "../header/mainview.h"
#include "../header/DataManagement.h"
#include "../header/FileManager.h"
#include "ui_mainview.h"
#include <QPixmap>
#include <QDebug>
#include <QResizeEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QSizePolicy>
#include <QTimer>
#include <QMouseEvent>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QPainter>
#include <QRect>
#include <QMenu>
#include <QInputDialog>
#include <cmath>
#include <set>
#include <utility>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
MainView::MainView(QWidget *parent) : QWidget(parent), ui(new Ui::MainView), isInFullscreen(false), 
                                       isDraggingConnection(false), dragSourceStationId(-1) {
    ui->setupUi(this);
    
    dataManagement = new DataManagement();
    fileManager = new FileManager();
    
    normalWindowSize = QSize(980, 641);  // Tamaño inicial de la ventana
    
    // Configurar el QLabel para que escale el contenido
    ui->lblInteractiveMap->setScaledContents(false);
    ui->lblInteractiveMap->setAlignment(Qt::AlignCenter);
    
    // Configurar el label de consulta de rutas
    ui->lblRoutesMap->setScaledContents(false);
    ui->lblRoutesMap->setAlignment(Qt::AlignCenter);
    ui->lblRoutesMap->installEventFilter(this);
    
    // Habilitar mouse tracking para el drag & drop
    setMouseTracking(true);
    
    // Conectar cambio de tabs
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == 1) { // Tab "Consulta de Rutas"
            isInRoutesTab = true;
            drawRoutesMap();
        } else {
            isInRoutesTab = false;
        }
    });
    
    // Inicializar botón deshabilitado
    ui->btnUseAlgorithm->setEnabled(false);
    
    loadMapImage();
}

MainView::~MainView() {
    delete fileManager;
    delete ui;
}

void MainView::loadMapImage() {
    QString imagePath = ":/assets/map.png";
    
    QPixmap pixmap(imagePath);
    
    if (pixmap.isNull()) {
        qWarning() << "Error: No se pudo cargar la imagen del mapa desde:" << imagePath;
        return;
    }
    
    originalPixmap = pixmap;
    
    // NO llamamos a resizeLabel() aquí porque el widget aún no tiene tamaño
    // Lo haremos en showEvent() cuando el widget esté visible y tenga su tamaño real
    
    qDebug() << "Imagen del mapa cargada:" << pixmap.width() << "x" << pixmap.height();
}
void MainView::resizeLabel() {
    if (originalPixmap.isNull()) {
        return;
    }
    
    QSize availableSize = ui->lblInteractiveMap->size();
    
    if (availableSize.width() <= 0 || availableSize.height() <= 0) {
        return;
    }
    
    qDebug() << "=== resizeLabel ===" << availableSize;
    
    // Dibujar las estaciones sobre el mapa (esto escalará internamente)
    drawStationsOnMap();
}

void MainView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    
    qDebug() << "resizeEvent triggered, new size:" << event->size();
    
    // Guardar el tamaño normal si no estamos en fullscreen
    if (!isFullScreen()) {
        normalWindowSize = event->size();
    }
    
    resizeLabel();
    
    if (event->oldSize().width() > 0) {
        QCoreApplication::processEvents();
    }
}

void MainView::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);
    
    if (event->type() == QEvent::WindowStateChange) {
        // Verificar el estado ANTERIOR para detectar cambios correctamente
        if (isFullScreen() && !isInFullscreen) {
            // Acabamos de entrar a fullscreen
            qDebug() << "Entrando a fullscreen";
            isInFullscreen = true;
        } else if (!isFullScreen() && isInFullscreen) {
            // Acabamos de salir de fullscreen
            qDebug() << "Saliendo de fullscreen";
            isInFullscreen = false;
            // Restaurar el tamaño normal
            QTimer::singleShot(0, this, [this]() {
                showNormal();
                resize(normalWindowSize);
                resizeLabel();
            });
        }
    }
}

void MainView::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    
    // Solo inicializamos una vez cuando el widget es visible por primera vez
    if (!isInitialized && !originalPixmap.isNull()) {
        qDebug() << "showEvent: Inicializando por primera vez";
        isInitialized = true;
        
        // Cargar datos antes de redimensionar
        loadAllData();
        
        // Usar un timer para asegurarnos de que el label tiene su tamaño final
        QTimer::singleShot(0, this, [this]() {
            resizeLabel();
            updateMapDisplay(); // Actualizar el mapa con los datos cargados
        });
    }
}

void MainView::mouseDoubleClickEvent(QMouseEvent *event) {
    // Deshabilitar en tab de consultas
    if (isInRoutesTab) {
        return;
    }
    
    if (originalPixmap.isNull()) {
        qDebug() << "No hay pixmap original";
        return;
    }
    
    // Asegurarnos de que los factores de escala estén actualizados
    if (scaledPixmapWithStations.isNull() || currentScaleX <= 0.01f || currentScaleY <= 0.01f) {
        qDebug() << "Recalculando factores de escala antes del click";
        drawStationsOnMap();
    }
    
    if (scaledPixmapWithStations.isNull()) {
        qDebug() << "No se pudo crear pixmap escalado";
        return;
    }
    
    // Posición relativa al label
    QPoint clickPosInLabel = ui->lblInteractiveMap->mapFromGlobal(event->globalPosition().toPoint());
    
    // Tamaños
    QSize labelSize = ui->lblInteractiveMap->size();
    QSize pixmapSize = scaledPixmapWithStations.size();
    
    // Calcular márgenes (el pixmap está centrado)
    int pixmapX = (labelSize.width() - pixmapSize.width()) / 2;
    int pixmapY = (labelSize.height() - pixmapSize.height()) / 2;
    
    // Convertir a coordenadas del pixmap escalado
    QPoint pixmapClickPos(clickPosInLabel.x() - pixmapX, clickPosInLabel.y() - pixmapY);
    
    // Verificar que está dentro del pixmap
    if (pixmapClickPos.x() < 0 || pixmapClickPos.y() < 0 || 
        pixmapClickPos.x() >= pixmapSize.width() || pixmapClickPos.y() >= pixmapSize.height()) {
        qDebug() << "Click fuera del pixmap";
        return;
    }
    
    // Convertir a coordenadas originales
    float originalX = pixmapClickPos.x() / currentScaleX;
    float originalY = pixmapClickPos.y() / currentScaleY;
    
    lastClickPos = QPointF(originalX, originalY);
    
    qDebug() << "\n=== DOUBLE CLICK ===";
    qDebug() << "Label size:" << labelSize;
    qDebug() << "Pixmap size:" << pixmapSize;
    qDebug() << "Margins:" << pixmapX << "," << pixmapY;
    qDebug() << "Click en label:" << clickPosInLabel;
    qDebug() << "Click en pixmap escalado:" << pixmapClickPos;
    qDebug() << "Scale factors:" << currentScaleX << "," << currentScaleY;
    qDebug() << "Coordenadas originales:" << originalX << "," << originalY;
    
    // Crear un diálogo personalizado para ingresar datos de la estación
    QDialog dialog(this);
    dialog.setWindowTitle("Nueva Estación");
    dialog.setModal(true);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    
    // Fila para ID
    QHBoxLayout *idLayout = new QHBoxLayout();
    QLabel *idLabel = new QLabel("ID (4 dígitos):", &dialog);
    QLineEdit *idInput = new QLineEdit(&dialog);
    
    // Validar que solo acepte 4 dígitos
    QRegularExpression idRegex("^\\d{0,4}$");
    QRegularExpressionValidator *idValidator = new QRegularExpressionValidator(idRegex, idInput);
    idInput->setValidator(idValidator);
    idInput->setMaxLength(4);
    idInput->setPlaceholderText("Ej: 1234");
    
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    mainLayout->addLayout(idLayout);
    
    // Fila para Nombre
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("Nombre:", &dialog);
    QLineEdit *nameInput = new QLineEdit(&dialog);
    nameInput->setPlaceholderText("Ej: Estación Centro");
    
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    mainLayout->addLayout(nameLayout);
    
    // Botones
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *acceptBtn = new QPushButton("Aceptar", &dialog);
    QPushButton *cancelBtn = new QPushButton("Cancelar", &dialog);
    
    buttonLayout->addWidget(acceptBtn);
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);
    
    // Conectar botones
    connect(acceptBtn, &QPushButton::clicked, [&]() {
        QString idStr = idInput->text().trimmed();
        QString name = nameInput->text().trimmed();
        
        // Validar que ambos campos estén llenos
        if (idStr.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(&dialog, "Campos Incompletos", 
                               "Por favor ingresa el ID y el nombre de la estación.");
            return;
        }
        
        // Validar que el ID tenga exactamente 4 dígitos
        if (idStr.length() != 4) {
            QMessageBox::warning(&dialog, "ID Inválido", 
                               "El ID debe contener exactamente 4 dígitos.");
            return;
        }
        
        int id = idStr.toInt();
        
        // Verificar si ya existe una estación con ese ID (ANTES de cerrar el diálogo)
        if (dataManagement->stationExists(id)) {
            QMessageBox::critical(&dialog, "Error: ID Duplicado", 
                                QString("Ya existe una estación con el ID %1.\nPor favor elige un ID diferente.").arg(id));
            return; // No cerrar el diálogo, permitir al usuario cambiar el ID
        }
        
        onStationCreated(id, name);
        dialog.accept();
    });
    
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    dialog.exec();
}

void MainView::onStationCreated(int id, const QString &name) {
    qDebug() << "Estación creada - ID:" << id << "Nombre:" << name 
             << "Coordenadas:" << lastClickPos;
    
    // Agregar la estación a DataManagement con las coordenadas
    dataManagement->addStation(id, name.toStdString(), lastClickPos.x(), lastClickPos.y());
    
    qDebug() << "Estación agregada a DataManagement";
    
    // Redibujar el mapa con la nueva estación
    updateMapDisplay();
}

void MainView::drawStationsOnMap() {
    if (originalPixmap.isNull()) {
        return;
    }
    
    // Obtener el tamaño disponible del label
    QSize availableSize = ui->lblInteractiveMap->size();
    if (availableSize.width() <= 0 || availableSize.height() <= 0) {
        return;
    }
    
    // Crear una copia limpia del pixmap escalado sin estaciones
    scaledPixmapWithStations = originalPixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // CALCULAR los factores de escala
    currentScaleX = (float)scaledPixmapWithStations.width() / originalPixmap.width();
    currentScaleY = (float)scaledPixmapWithStations.height() / originalPixmap.height();
    
    // Obtener todas las estaciones
    vector<Station> stations = dataManagement->getAllStations();
    
    qDebug() << "=== drawStationsOnMap ===";
    qDebug() << "Label size:" << availableSize;
    qDebug() << "Scaled pixmap size:" << scaledPixmapWithStations.size();
    qDebug() << "Original pixmap size:" << originalPixmap.size();
    qDebug() << "Scale factors - X:" << currentScaleX << "Y:" << currentScaleY;
    qDebug() << "Total estaciones:" << stations.size();
    
    // Si no hay estaciones, solo mostrar el mapa
    if (stations.empty()) {
        ui->lblInteractiveMap->setPixmap(scaledPixmapWithStations);
        return;
    }
    
    // Crear un painter para dibujar sobre el pixmap
    QPainter painter(&scaledPixmapWithStations);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    
    // PRIMERO: Dibujar todas las conexiones (líneas)
    const Grafo& grafo = dataManagement->getGrafo();
    std::set<std::pair<int, int>> drawnConnections; // Para evitar dibujar la misma línea dos veces
    
    // Configurar para dibujar líneas - azul más oscuro
    painter.setPen(QPen(QColor(30, 80, 150), 2)); // Azul más oscuro
    
    for (const auto& pair : grafo) {
        int stationId = pair.first;
        const auto& adjacents = pair.second;
        
        // Obtener coordenadas de la estación origen
        auto coordsFrom = dataManagement->getCoordinates(stationId);
        float fromX = coordsFrom.first * currentScaleX;
        float fromY = coordsFrom.second * currentScaleY;
        
        // Dibujar línea a cada estación adyacente
        for (const auto& adjacent : adjacents) {
            int adjacentId = adjacent.first;
            float weight = adjacent.second; // Peso (tiempo) de la arista
            
            // Evitar dibujar la misma conexión dos veces (grafo no dirigido)
            std::pair<int, int> connection = stationId < adjacentId 
                ? std::make_pair(stationId, adjacentId)
                : std::make_pair(adjacentId, stationId);
            
            if (drawnConnections.find(connection) != drawnConnections.end()) {
                continue; // Ya se dibujó esta conexión
            }
            
            // Obtener coordenadas de la estación destino
            auto coordsTo = dataManagement->getCoordinates(adjacentId);
            float toX = coordsTo.first * currentScaleX;
            float toY = coordsTo.second * currentScaleY;
            
            // Verificar si la ruta está cerrada
            bool isClosed = dataManagement->isRouteClosed(stationId, adjacentId);
            
            // Configurar color según el estado de la ruta
            if (isClosed) {
                painter.setPen(QPen(QColor(200, 30, 30), 2)); // Rojo para rutas cerradas
            } else {
                painter.setPen(QPen(QColor(30, 80, 150), 2)); // Azul para rutas abiertas
            }
            
            // Dibujar la línea
            painter.drawLine(QPointF(fromX, fromY), QPointF(toX, toY));
            
            // Calcular punto medio de la línea
            float midX = (fromX + toX) / 2.0f;
            float midY = (fromY + toY) / 2.0f;
            
            // Preparar texto del peso (tiempo en minutos)
            QString weightText = QString::number(weight, 'f', 1) + " min";
            
            // Configurar fuente para el texto del peso
            QFont weightFont = painter.font();
            weightFont.setPointSize(7);
            weightFont.setBold(true);
            painter.setFont(weightFont);
            
            QFontMetrics metrics(weightFont);
            int textWidth = metrics.horizontalAdvance(weightText);
            int textHeight = metrics.height();
            
            // Calcular posición del texto (centrado en el punto medio)
            int textX = midX - textWidth / 2;
            int textY = midY + metrics.ascent() / 2;
            
            // Dibujar fondo semi-transparente para el texto
            QRect bgRect(textX - 3, textY - metrics.ascent() - 1, 
                        textWidth + 6, textHeight + 2);
            painter.fillRect(bgRect, QColor(0, 0, 0, 180)); // Fondo negro semi-transparente
            
            // Dibujar el texto del peso en blanco
            painter.setPen(QPen(Qt::white));
            painter.drawText(textX, textY, weightText);
            
            // Restaurar pen para las líneas
            painter.setPen(QPen(QColor(30, 80, 150), 2));
            
            // Marcar como dibujada
            drawnConnections.insert(connection);
        }
    }
    
    // SEGUNDO: Dibujar cada estación usando el map de coords
    for (const Station& station : stations) {
        // Obtener coordenadas del map (separación de responsabilidades)
        auto coords = dataManagement->getCoordinates(station.id);
        float originalX = coords.first;
        float originalY = coords.second;
        
        // Escalar coordenadas
        float scaledX = originalX * currentScaleX;
        float scaledY = originalY * currentScaleY;
        
        qDebug() << "Estación" << station.id << ":" << station.name.c_str()
                 << "Original(" << originalX << "," << originalY << ")"
                 << "Escalado(" << scaledX << "," << scaledY << ")";
        
        // Dibujar círculo azul (más pequeño)
        painter.setBrush(QBrush(Qt::blue));
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(QPointF(scaledX, scaledY), 8, 8);
        
        // Dibujar nombre de la estación debajo del punto
        painter.setPen(QPen(Qt::white));
        QFont font = painter.font();
        font.setPointSize(8);  // Letra pequeña
        font.setBold(true);    // Negrita para mejor legibilidad
        painter.setFont(font);
        
        // Calcular posición del texto (centrado debajo del círculo)
        QString stationName = QString::fromStdString(station.name);
        QFontMetrics metrics(font);
        int textWidth = metrics.horizontalAdvance(stationName);
        int textX = scaledX - textWidth / 2;
        int textY = scaledY + 20;  // 20 píxeles debajo del centro del círculo
        
        // Dibujar fondo negro semi-transparente para el texto (mejor legibilidad)
        QRect textRect(textX - 2, textY - metrics.ascent() - 1, 
                       textWidth + 4, metrics.height() + 2);
        painter.fillRect(textRect, QColor(0, 0, 0, 150));
        
        // Dibujar el texto
        painter.drawText(textX, textY, stationName);
    }
    
    painter.end();
    
    // Actualizar el label
    ui->lblInteractiveMap->setPixmap(scaledPixmapWithStations);
}

void MainView::updateMapDisplay() {
    drawStationsOnMap();
}

void MainView::mousePressEvent(QMouseEvent *event) {
    // Deshabilitar en tab de consultas
    if (isInRoutesTab) {
        QWidget::mousePressEvent(event);
        return;
    }
    
    // Verificar que el clic es dentro del label del mapa
    QPoint clickPosInLabel = ui->lblInteractiveMap->mapFromGlobal(event->globalPosition().toPoint());
    QSize labelSize = ui->lblInteractiveMap->size();
    QPixmap currentPixmap = ui->lblInteractiveMap->pixmap();
    
    if (currentPixmap.isNull()) {
        QWidget::mousePressEvent(event);
        return;
    }
    
    // Calcular el offset del pixmap dentro del label (por el AlignCenter)
    int pixmapX = (labelSize.width() - currentPixmap.width()) / 2;
    int pixmapY = (labelSize.height() - currentPixmap.height()) / 2;
    
    // Verificar si el clic está dentro del pixmap
    if (clickPosInLabel.x() < pixmapX || clickPosInLabel.x() > pixmapX + currentPixmap.width() ||
        clickPosInLabel.y() < pixmapY || clickPosInLabel.y() > pixmapY + currentPixmap.height()) {
        QWidget::mousePressEvent(event);
        return;
    }
    
    // Convertir a coordenadas del pixmap escalado
    QPointF pixmapClickPos(clickPosInLabel.x() - pixmapX, clickPosInLabel.y() - pixmapY);
    
    // Buscar estación en esa posición
    int stationId = getStationAtPosition(pixmapClickPos);
    
    if (stationId != -1) {
        if (event->button() == Qt::LeftButton) {
            // Iniciar drag para crear conexión
            isDraggingConnection = true;
            dragSourceStationId = stationId;
            dragCurrentPos = event->pos();
            setCursor(Qt::CrossCursor);
            qDebug() << "Iniciando drag desde estación" << stationId;
        } else if (event->button() == Qt::RightButton) {
            // Mostrar menú contextual de estación
            showStationContextMenu(stationId, event->globalPosition().toPoint());
        }
    } else if (event->button() == Qt::RightButton) {
        // No hay estación, verificar si se hizo clic en una conexión
        auto connection = getConnectionAtPosition(pixmapClickPos);
        if (connection.first != -1 && connection.second != -1) {
            // Se hizo clic en una conexión
            showConnectionContextMenu(connection.first, connection.second, event->globalPosition().toPoint());
        }
    } else {
        QWidget::mousePressEvent(event);
    }
}

int MainView::getStationAtPosition(const QPointF &pos) {
    // Obtener todas las estaciones
    vector<Station> stations = dataManagement->getAllStations();
    
    // Radio de detección (mismo que el radio del círculo + un margen)
    const float detectionRadius = 12.0f;
    
    // Buscar estación más cercana dentro del radio
    for (const Station& station : stations) {
        auto coords = dataManagement->getCoordinates(station.id);
        float scaledX = coords.first * currentScaleX;
        float scaledY = coords.second * currentScaleY;
        
        // Calcular distancia
        float dx = pos.x() - scaledX;
        float dy = pos.y() - scaledY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance <= detectionRadius) {
            return station.id;
        }
    }
    
    return -1; // No se encontró ninguna estación
}

void MainView::showStationContextMenu(int stationId, const QPoint &globalPos) {
    Station* station = dataManagement->getStation(stationId);
    if (!station) {
        return;
    }
    
    QMenu contextMenu(this);
    
    // Agregar título con el nombre de la estación
    QAction *titleAction = contextMenu.addAction(QString("Estación: %1").arg(QString::fromStdString(station->name)));
    titleAction->setEnabled(false); // No clickeable, solo informativo
    
    contextMenu.addSeparator();
    
    // Opción: Cambiar nombre
    QAction *renameAction = contextMenu.addAction("Cambiar nombre");
    
    // Opción: Eliminar
    QAction *deleteAction = contextMenu.addAction("Eliminar estación");
    
    // Ejecutar menú y procesar acción seleccionada
    QAction *selectedAction = contextMenu.exec(globalPos);
    
    if (selectedAction == renameAction) {
        renameStation(stationId);
    } else if (selectedAction == deleteAction) {
        deleteStation(stationId);
    }
}

void MainView::renameStation(int stationId) {
    Station* station = dataManagement->getStation(stationId);
    if (!station) {
        return;
    }
    
    bool ok;
    QString newName = QInputDialog::getText(this, 
                                           "Cambiar Nombre de Estación",
                                           QString("ID: %1\nNombre actual: %2\n\nNuevo nombre:")
                                               .arg(stationId)
                                               .arg(QString::fromStdString(station->name)),
                                           QLineEdit::Normal,
                                           QString::fromStdString(station->name),
                                           &ok);
    
    if (!ok || newName.trimmed().isEmpty()) {
        return; // Usuario canceló o no ingresó nombre
    }
    
    // Pedir confirmación
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, 
                                  "Confirmar Cambio",
                                  QString("¿Estás seguro de cambiar el nombre de la estación?\n\n"
                                         "ID: %1\n"
                                         "Nombre anterior: %2\n"
                                         "Nombre nuevo: %3")
                                      .arg(stationId)
                                      .arg(QString::fromStdString(station->name))
                                      .arg(newName.trimmed()),
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        dataManagement->updateStationName(stationId, newName.trimmed().toStdString());
        updateMapDisplay();
    }
}

void MainView::deleteStation(int stationId) {
    Station* station = dataManagement->getStation(stationId);
    if (!station) {
        return;
    }
    
    // Pedir confirmación
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this,
                                "Confirmar Eliminación",
                                QString("¿Estás seguro de eliminar esta estación?\n\n"
                                       "ID: %1\n"
                                       "Nombre: %2\n\n"
                                       "Esta acción no se puede deshacer.")
                                    .arg(stationId)
                                    .arg(QString::fromStdString(station->name)),
                                QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        dataManagement->removeStation(stationId);
        updateMapDisplay();
    }
}

void MainView::mouseMoveEvent(QMouseEvent *event) {
    // Deshabilitar en tab de consultas
    if (isInRoutesTab) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    
    if (isDraggingConnection) {
        dragCurrentPos = event->pos();
        
        // Redibujar el mapa con la línea temporal
        drawStationsOnMap();
        
        // Dibujar la línea de drag en el pixmap actual
        QPixmap tempPixmap = scaledPixmapWithStations;
        QPainter painter(&tempPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Obtener coordenadas de la estación origen en el pixmap escalado
        auto coords = dataManagement->getCoordinates(dragSourceStationId);
        float scaledX = coords.first * currentScaleX;
        float scaledY = coords.second * currentScaleY;
        
        // Convertir posición del mouse a coordenadas del pixmap
        QPoint labelPos = ui->lblInteractiveMap->mapFromGlobal(mapToGlobal(dragCurrentPos));
        QSize labelSize = ui->lblInteractiveMap->size();
        
        // Calcular offset del pixmap en el label
        int pixmapOffsetX = (labelSize.width() - tempPixmap.width()) / 2;
        int pixmapOffsetY = (labelSize.height() - tempPixmap.height()) / 2;
        
        // Coordenadas relativas al pixmap
        int mousePixmapX = labelPos.x() - pixmapOffsetX;
        int mousePixmapY = labelPos.y() - pixmapOffsetY;
        
        // Dibujar línea azul clara
        QPen pen(QColor(100, 150, 255), 3);
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.drawLine(QPointF(scaledX, scaledY), QPointF(mousePixmapX, mousePixmapY));
        
        painter.end();
        
        // Actualizar el label
        ui->lblInteractiveMap->setPixmap(tempPixmap);
        
        qDebug() << "Drag moviendo desde" << dragSourceStationId 
                 << "hacia (" << mousePixmapX << "," << mousePixmapY << ")";
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void MainView::mouseReleaseEvent(QMouseEvent *event) {
    // Deshabilitar en tab de consultas
    if (isInRoutesTab) {
        QWidget::mouseReleaseEvent(event);
        return;
    }
    
    if (event->button() == Qt::LeftButton && isDraggingConnection) {
        qDebug() << "Mouse released, buscando estación destino...";
        
        // Convertir posición del mouse a coordenadas del pixmap
        QPoint labelPos = ui->lblInteractiveMap->mapFromGlobal(event->globalPosition().toPoint());
        QSize labelSize = ui->lblInteractiveMap->size();
        QPixmap currentPixmap = ui->lblInteractiveMap->pixmap();
        
        if (!currentPixmap.isNull()) {
            // Calcular offset del pixmap en el label
            int pixmapOffsetX = (labelSize.width() - currentPixmap.width()) / 2;
            int pixmapOffsetY = (labelSize.height() - currentPixmap.height()) / 2;
            
            // Coordenadas relativas al pixmap
            QPointF pixmapPos(labelPos.x() - pixmapOffsetX, labelPos.y() - pixmapOffsetY);
            
            qDebug() << "Posición en pixmap:" << pixmapPos;
            
            // Buscar estación en la posición de suelta
            int targetStationId = getStationAtPosition(pixmapPos);
            
            qDebug() << "Estación destino encontrada:" << targetStationId;
            
            if (targetStationId != -1 && targetStationId != dragSourceStationId) {
                // Se soltó sobre una estación diferente, intentar crear conexión
                addConnection(dragSourceStationId, targetStationId);
            } else if (targetStationId == -1) {
                qDebug() << "No se soltó sobre ninguna estación";
            }
        }
        
        // Finalizar drag y redibujar mapa limpio
        isDraggingConnection = false;
        dragSourceStationId = -1;
        setCursor(Qt::ArrowCursor);
        updateMapDisplay(); // Redibujar mapa completo sin línea temporal
        
        qDebug() << "Drag finalizado";
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

void MainView::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    // La línea de drag ahora se dibuja directamente en el pixmap durante mouseMoveEvent
}

QPointF MainView::widgetPosToPixmapPos(const QPoint &widgetPos) {
    // Convertir posición del widget a coordenadas del pixmap
    QPoint clickPosInLabel = ui->lblInteractiveMap->mapFromParent(widgetPos);
    QSize labelSize = ui->lblInteractiveMap->size();
    QPixmap currentPixmap = ui->lblInteractiveMap->pixmap();
    
    if (currentPixmap.isNull()) {
        return QPointF(-1, -1);
    }
    
    // Calcular el offset del pixmap dentro del label
    int pixmapX = (labelSize.width() - currentPixmap.width()) / 2;
    int pixmapY = (labelSize.height() - currentPixmap.height()) / 2;
    
    // Convertir a coordenadas del pixmap escalado
    return QPointF(clickPosInLabel.x() - pixmapX, clickPosInLabel.y() - pixmapY);
}

void MainView::addConnection(int stationA, int stationB) {
    // Verificar que ambas estaciones existen
    if (!dataManagement->stationExists(stationA) || !dataManagement->stationExists(stationB)) {
        qDebug() << "Error: Una o ambas estaciones no existen";
        return;
    }
    
    // Obtener nombres de las estaciones
    Station* stA = dataManagement->getStation(stationA);
    Station* stB = dataManagement->getStation(stationB);
    
    if (!stA || !stB) {
        return;
    }
    
    // Verificar si ya existe la conexión
    if (dataManagement->routeExists(stationA, stationB)) {
        QMessageBox::warning(this, "Conexión Existente",
                           QString("Ya existe una conexión entre:\n\n"
                                  "• %1 (ID: %2)\n"
                                  "• %3 (ID: %4)")
                               .arg(QString::fromStdString(stA->name))
                               .arg(stationA)
                               .arg(QString::fromStdString(stB->name))
                               .arg(stationB));
        qDebug() << "Conexión ya existe entre" << stationA << "y" << stationB;
        return;
    }
    
    // Calcular distancia euclidiana entre las estaciones (en píxeles)
    auto coordsA = dataManagement->getCoordinates(stationA);
    auto coordsB = dataManagement->getCoordinates(stationB);
    
    float dx = coordsB.first - coordsA.first;
    float dy = coordsB.second - coordsA.second;
    float pixelDistance = std::sqrt(dx * dx + dy * dy);
    
    // Calcular tiempo estimado en minutos basado en la distancia
    // Fórmula: cada 100 píxeles = 5 minutos
    // Esto da números razonables: 200px = 10min, 300px = 15min, etc.
    float travelTime = (pixelDistance / 100.0f) * 5.0f;
    
    // Redondear a 0.5 minutos para números más bonitos
    travelTime = std::round(travelTime * 2.0f) / 2.0f;
    
    // Mínimo 0.5 minutos para distancias muy cortas
    if (travelTime < 0.5f) {
        travelTime = 0.5f;
    }
    
    // Pedir confirmación para crear la ruta
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                 "Crear Conexión",
                                 QString("¿Deseas crear una conexión entre?\n\n"
                                        "• %1 (ID: %2)\n"
                                        "• %3 (ID: %4)\n\n"
                                        "Distancia: %5 px\n"
                                        "Tiempo estimado: %6 min")
                                     .arg(QString::fromStdString(stA->name))
                                     .arg(stationA)
                                     .arg(QString::fromStdString(stB->name))
                                     .arg(stationB)
                                     .arg(QString::number(pixelDistance, 'f', 1))
                                     .arg(QString::number(travelTime, 'f', 1)),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Agregar la ruta con el tiempo como peso (bidireccional)
        dataManagement->addRoute(stationA, stationB, travelTime);
        
        qDebug() << "Conexión creada entre" << stationA << "y" << stationB 
                 << "- Distancia:" << pixelDistance << "px"
                 << "- Tiempo:" << travelTime << "min";
        
        // Redibujar el mapa con la nueva conexión
        updateMapDisplay();
    } else {
        qDebug() << "Creación de conexión cancelada por el usuario";
    }
}

std::pair<int, int> MainView::getConnectionAtPosition(const QPointF &pos) {
    const Grafo& grafo = dataManagement->getGrafo();
    std::set<std::pair<int, int>> checkedConnections;
    
    // Distancia máxima para detectar clic en línea (en píxeles)
    const float maxDistance = 8.0f;
    
    for (const auto& pair : grafo) {
        int stationId = pair.first;
        const auto& adjacents = pair.second;
        
        // Obtener coordenadas de la estación origen
        auto coordsFrom = dataManagement->getCoordinates(stationId);
        float fromX = coordsFrom.first * currentScaleX;
        float fromY = coordsFrom.second * currentScaleY;
        
        for (const auto& adjacent : adjacents) {
            int adjacentId = adjacent.first;
            
            // Evitar verificar la misma conexión dos veces
            std::pair<int, int> connection = stationId < adjacentId 
                ? std::make_pair(stationId, adjacentId)
                : std::make_pair(adjacentId, stationId);
            
            if (checkedConnections.find(connection) != checkedConnections.end()) {
                continue;
            }
            checkedConnections.insert(connection);
            
            // Obtener coordenadas de la estación destino
            auto coordsTo = dataManagement->getCoordinates(adjacentId);
            float toX = coordsTo.first * currentScaleX;
            float toY = coordsTo.second * currentScaleY;
            
            // Calcular distancia del punto a la línea
            // Usando fórmula de distancia punto-segmento
            float dx = toX - fromX;
            float dy = toY - fromY;
            float lengthSquared = dx * dx + dy * dy;
            
            if (lengthSquared == 0) continue; // Puntos coincidentes
            
            // Proyección del punto sobre la línea (t entre 0 y 1 si está en el segmento)
            float posXf = static_cast<float>(pos.x());
            float posYf = static_cast<float>(pos.y());
            float t = std::max(0.0f, std::min(1.0f, 
                ((posXf - fromX) * dx + (posYf - fromY) * dy) / lengthSquared));
            
            // Punto más cercano en la línea
            float nearestX = fromX + t * dx;
            float nearestY = fromY + t * dy;
            
            // Distancia del clic al punto más cercano en la línea
            float distX = posXf - nearestX;
            float distY = posYf - nearestY;
            float distance = std::sqrt(distX * distX + distY * distY);
            
            if (distance <= maxDistance) {
                // Se hizo clic cerca de esta línea
                return std::make_pair(stationId, adjacentId);
            }
        }
    }
    
    return std::make_pair(-1, -1); // No se encontró ninguna conexión
}

void MainView::showConnectionContextMenu(int stationA, int stationB, const QPoint &globalPos) {
    Station* stA = dataManagement->getStation(stationA);
    Station* stB = dataManagement->getStation(stationB);
    
    if (!stA || !stB) {
        return;
    }
    
    QMenu contextMenu(this);
    
    // Título del menú
    QAction *titleAction = contextMenu.addAction(
        QString("Ruta: %1 ↔ %2")
            .arg(QString::fromStdString(stA->name))
            .arg(QString::fromStdString(stB->name))
    );
    titleAction->setEnabled(false);
    
    contextMenu.addSeparator();
    
    // Verificar estado actual
    bool isClosed = dataManagement->isRouteClosed(stationA, stationB);
    
    QAction *toggleAction;
    if (isClosed) {
        toggleAction = contextMenu.addAction("🟢 Abrir vía");
    } else {
        toggleAction = contextMenu.addAction("🔴 Cerrar vía");
    }
    
    // Ejecutar menú
    QAction *selectedAction = contextMenu.exec(globalPos);
    
    if (selectedAction == toggleAction) {
        toggleRouteStatus(stationA, stationB);
    }
}

void MainView::toggleRouteStatus(int stationA, int stationB) {
    Station* stA = dataManagement->getStation(stationA);
    Station* stB = dataManagement->getStation(stationB);
    
    if (!stA || !stB) {
        return;
    }
    
    bool isClosed = dataManagement->isRouteClosed(stationA, stationB);
    
    if (isClosed) {
        // Abrir la ruta
        dataManagement->openRoute(stationA, stationB);
        qDebug() << "Ruta abierta entre" << stationA << "y" << stationB;
    } else {
        // Cerrar la ruta
        dataManagement->closeRoute(stationA, stationB);
        qDebug() << "Ruta cerrada entre" << stationA << "y" << stationB;
    }
    
    // Redibujar el mapa con el nuevo estado
    updateMapDisplay();
}

// ==================== CONSULTA DE RUTAS ====================

bool MainView::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->lblRoutesMap && event->type() == QEvent::MouseButtonPress && isInRoutesTab) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QPoint clickPos = mouseEvent->pos();
            
            // Convertir a coordenadas del pixmap
            QSize labelSize = ui->lblRoutesMap->size();
            QPixmap currentPixmap = ui->lblRoutesMap->pixmap();
            
            if (!currentPixmap.isNull()) {
                int pixmapOffsetX = (labelSize.width() - currentPixmap.width()) / 2;
                int pixmapOffsetY = (labelSize.height() - currentPixmap.height()) / 2;
                
                QPointF pixmapPos(clickPos.x() - pixmapOffsetX, clickPos.y() - pixmapOffsetY);
                handleRoutesMapClick(pixmapPos);
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainView::drawRoutesMap() {
    if (originalPixmap.isNull()) {
        return;
    }
    
    QSize availableSize = ui->lblRoutesMap->size();
    if (availableSize.width() <= 0 || availableSize.height() <= 0) {
        return;
    }
    
    // Crear pixmap escalado
    routesPixmap = originalPixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    routesScaleX = (float)routesPixmap.width() / originalPixmap.width();
    routesScaleY = (float)routesPixmap.height() / originalPixmap.height();
    
    vector<Station> stations = dataManagement->getAllStations();
    
    if (stations.empty()) {
        ui->lblRoutesMap->setPixmap(routesPixmap);
        return;
    }
    
    QPainter painter(&routesPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    
    // Dibujar conexiones
    const Grafo& grafo = dataManagement->getGrafo();
    std::set<std::pair<int, int>> drawnConnections;
    
    for (const auto& pair : grafo) {
        int stationId = pair.first;
        const auto& adjacents = pair.second;
        
        auto coordsFrom = dataManagement->getCoordinates(stationId);
        float fromX = coordsFrom.first * routesScaleX;
        float fromY = coordsFrom.second * routesScaleY;
        
        for (const auto& adjacent : adjacents) {
            int adjacentId = adjacent.first;
            float weight = adjacent.second;
            
            std::pair<int, int> connection = stationId < adjacentId 
                ? std::make_pair(stationId, adjacentId)
                : std::make_pair(adjacentId, stationId);
            
            if (drawnConnections.find(connection) != drawnConnections.end()) {
                continue;
            }
            
            auto coordsTo = dataManagement->getCoordinates(adjacentId);
            float toX = coordsTo.first * routesScaleX;
            float toY = coordsTo.second * routesScaleY;
            
            bool isClosed = dataManagement->isRouteClosed(stationId, adjacentId);
            
            if (isClosed) {
                painter.setPen(QPen(QColor(200, 30, 30), 2));
            } else {
                painter.setPen(QPen(QColor(30, 80, 150), 2));
            }
            
            painter.drawLine(QPointF(fromX, fromY), QPointF(toX, toY));
            
            // Dibujar peso
            float midX = (fromX + toX) / 2.0f;
            float midY = (fromY + toY) / 2.0f;
            QString weightText = QString::number(weight, 'f', 1) + " min";
            
            QFont weightFont = painter.font();
            weightFont.setPointSize(7);
            weightFont.setBold(true);
            painter.setFont(weightFont);
            
            QFontMetrics metrics(weightFont);
            int textWidth = metrics.horizontalAdvance(weightText);
            int textHeight = metrics.height();
            int textX = midX - textWidth / 2;
            int textY = midY + metrics.ascent() / 2;
            
            QRect bgRect(textX - 3, textY - metrics.ascent() - 1, 
                        textWidth + 6, textHeight + 2);
            painter.fillRect(bgRect, QColor(0, 0, 0, 180));
            painter.setPen(QPen(Qt::white));
            painter.drawText(textX, textY, weightText);
            
            drawnConnections.insert(connection);
        }
    }
    
    // Dibujar estaciones
    for (const Station& station : stations) {
        auto coords = dataManagement->getCoordinates(station.id);
        float scaledX = coords.first * routesScaleX;
        float scaledY = coords.second * routesScaleY;
        
        // Color según selección
        QColor stationColor = Qt::blue;
        QString label = "";
        
        if (station.id == originStationId) {
            stationColor = QColor(0, 180, 0); // Verde para origen
            label = "ORIGEN";
        } else if (station.id == destinationStationId) {
            stationColor = QColor(0, 180, 0); // Verde para destino
            label = "DESTINO";
        }
        
        painter.setBrush(QBrush(stationColor));
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(QPointF(scaledX, scaledY), 8, 8);
        
        // Dibujar nombre
        painter.setPen(QPen(Qt::white));
        QFont font = painter.font();
        font.setPointSize(8);
        font.setBold(true);
        painter.setFont(font);
        
        QString stationName = QString::fromStdString(station.name);
        QFontMetrics metrics(font);
        int textWidth = metrics.horizontalAdvance(stationName);
        int textX = scaledX - textWidth / 2;
        int textY = scaledY + 20;
        
        QRect textRect(textX - 2, textY - metrics.ascent() - 1, 
                       textWidth + 4, metrics.height() + 2);
        painter.fillRect(textRect, QColor(0, 0, 0, 150));
        painter.drawText(textX, textY, stationName);
        
        // Dibujar etiqueta origen/destino
        if (!label.isEmpty()) {
            font.setPointSize(6);
            painter.setFont(font);
            QFontMetrics labelMetrics(font);
            int labelWidth = labelMetrics.horizontalAdvance(label);
            int labelX = scaledX - labelWidth / 2;
            int labelY = scaledY - 12;
            
            QRect labelRect(labelX - 2, labelY - labelMetrics.ascent() - 1,
                           labelWidth + 4, labelMetrics.height() + 2);
            painter.fillRect(labelRect, QColor(0, 180, 0, 200));
            painter.drawText(labelX, labelY, label);
        }
    }
    
    painter.end();
    ui->lblRoutesMap->setPixmap(routesPixmap);
}

void MainView::handleRoutesMapClick(const QPointF &pos) {
    int stationId = getStationAtPositionRoutes(pos);
    
    if (stationId == -1) {
        return;
    }
    
    QString currentAlgo = ui->comboBox->currentText();
    
    // Para Prim y Kruskal, limpiar selección automáticamente
    if (currentAlgo == "Prim" || currentAlgo == "Kruskal") {
        clearSelection();
        return;
    }
    
    // Para Dijkstra y Floyd-Warshall
    if (originStationId == -1) {
        selectOrigin(stationId);
    } else if (destinationStationId == -1 && stationId != originStationId) {
        selectDestination(stationId);
    } else {
        // Ya hay origen y destino, reiniciar selección
        clearSelection();
        selectOrigin(stationId);
    }
}

int MainView::getStationAtPositionRoutes(const QPointF &pos) {
    vector<Station> stations = dataManagement->getAllStations();
    const float detectionRadius = 12.0f;
    
    for (const Station& station : stations) {
        auto coords = dataManagement->getCoordinates(station.id);
        float scaledX = coords.first * routesScaleX;
        float scaledY = coords.second * routesScaleY;
        
        float dx = pos.x() - scaledX;
        float dy = pos.y() - scaledY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance <= detectionRadius) {
            return station.id;
        }
    }
    
    return -1;
}

void MainView::selectOrigin(int stationId) {
    originStationId = stationId;
    qDebug() << "Origen seleccionado:" << stationId;
    updateButtonStates();
    drawRoutesMap();
}

void MainView::selectDestination(int stationId) {
    destinationStationId = stationId;
    qDebug() << "Destino seleccionado:" << stationId;
    updateButtonStates();
    drawRoutesMap();
}

void MainView::clearSelection() {
    originStationId = -1;
    destinationStationId = -1;
    algorithmPath.clear();
    totalPathWeight = 0.0f;
    updateButtonStates();
    drawRoutesMap();
}

void MainView::updateButtonStates() {
    QString currentAlgo = ui->comboBox->currentText();
    
    if (currentAlgo == "Dijkstra" || currentAlgo == "Floyd-Warshall") {
        // Requieren origen y destino
        ui->btnUseAlgorithm->setEnabled(originStationId != -1 && destinationStationId != -1);
    } else if (currentAlgo == "Prim" || currentAlgo == "Kruskal") {
        // No requieren selección
        ui->btnUseAlgorithm->setEnabled(true);
    }
}

void MainView::on_comboBox_currentIndexChanged(int index) {
    QString algo = ui->comboBox->currentText();
    
    // Si cambiamos a Prim o Kruskal, limpiar selección
    if (algo == "Prim" || algo == "Kruskal") {
        clearSelection();
    }
    
    updateButtonStates();
}

void MainView::on_btnUseAlgorithm_clicked() {
    QString algo = ui->comboBox->currentText();
    
    // Incrementar contador de consultas
    dataManagement->incrementQueryCount();
    
    if (algo == "Dijkstra") {
        runDijkstra();
    } else if (algo == "Floyd-Warshall") {
        runFloydWarshall();
    } else if (algo == "Prim") {
        runPrim();
    } else if (algo == "Kruskal") {
        runKruskal();
    }
}

void MainView::on_btnReset_clicked() {
    clearSelection();
}

void MainView::runDijkstra() {
    if (originStationId == -1 || destinationStationId == -1) {
        return;
    }
    
    qDebug() << "Ejecutando Dijkstra desde" << originStationId << "hasta" << destinationStationId;
    
    // Implementar Dijkstra
    const Grafo& grafo = dataManagement->getGrafo();
    std::map<int, float> dist;
    std::map<int, int> prev;
    std::set<int> visited;
    
    // Inicializar distancias
    for (const auto& pair : grafo) {
        dist[pair.first] = std::numeric_limits<float>::infinity();
    }
    dist[originStationId] = 0.0f;
    
    // Priority queue: {distancia, nodo}
    std::priority_queue<std::pair<float, int>, 
                       std::vector<std::pair<float, int>>,
                       std::greater<std::pair<float, int>>> pq;
    pq.push({0.0f, originStationId});
    
    while (!pq.empty()) {
        int current = pq.top().second;
        float currentDist = pq.top().first;
        pq.pop();
        
        if (visited.count(current)) continue;
        visited.insert(current);
        
        if (current == destinationStationId) break;
        
        // Revisar vecinos
        auto adjacents = dataManagement->getAdjacentStations(current);
        for (const auto& adj : adjacents) {
            int neighbor = adj.first;
            float weight = adj.second;
            
            // Ignorar rutas cerradas
            if (dataManagement->isRouteClosed(current, neighbor)) {
                continue;
            }
            
            float newDist = currentDist + weight;
            if (newDist < dist[neighbor]) {
                dist[neighbor] = newDist;
                prev[neighbor] = current;
                pq.push({newDist, neighbor});
            }
        }
    }
    
    // Reconstruir camino
    algorithmPath.clear();
    if (dist[destinationStationId] == std::numeric_limits<float>::infinity()) {
        QMessageBox::warning(this, "Sin Ruta", "No existe un camino entre las estaciones seleccionadas.");
        return;
    }
    
    int current = destinationStationId;
    while (current != originStationId) {
        algorithmPath.insert(algorithmPath.begin(), current);
        current = prev[current];
    }
    algorithmPath.insert(algorithmPath.begin(), originStationId);
    
    totalPathWeight = dist[destinationStationId];
    
    qDebug() << "Camino encontrado con peso total:" << totalPathWeight;
    
    drawPathResult();
}

void MainView::runFloydWarshall() {
    if (originStationId == -1 || destinationStationId == -1) {
        return;
    }
    
    qDebug() << "Ejecutando Floyd-Warshall desde" << originStationId << "hasta" << destinationStationId;
    
    // Implementar Floyd-Warshall
    const Grafo& grafo = dataManagement->getGrafo();
    std::vector<int> nodes;
    std::map<int, int> nodeIndex;
    
    // Crear lista de nodos
    for (const auto& pair : grafo) {
        nodeIndex[pair.first] = nodes.size();
        nodes.push_back(pair.first);
    }
    
    int n = nodes.size();
    std::vector<std::vector<float>> dist(n, std::vector<float>(n, std::numeric_limits<float>::infinity()));
    std::vector<std::vector<int>> next(n, std::vector<int>(n, -1));
    
    // Inicializar distancias
    for (int i = 0; i < n; i++) {
        dist[i][i] = 0.0f;
    }
    
    for (const auto& pair : grafo) {
        int from = nodeIndex[pair.first];
        for (const auto& adj : pair.second) {
            int to = nodeIndex[adj.first];
            float weight = adj.second;
            
            if (!dataManagement->isRouteClosed(pair.first, adj.first)) {
                dist[from][to] = weight;
                next[from][to] = to;
            }
        }
    }
    
    // Floyd-Warshall
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
    
    // Reconstruir camino
    int fromIdx = nodeIndex[originStationId];
    int toIdx = nodeIndex[destinationStationId];
    
    if (dist[fromIdx][toIdx] == std::numeric_limits<float>::infinity()) {
        QMessageBox::warning(this, "Sin Ruta", "No existe un camino entre las estaciones seleccionadas.");
        return;
    }
    
    algorithmPath.clear();
    int current = fromIdx;
    while (current != toIdx) {
        algorithmPath.push_back(nodes[current]);
        current = next[current][toIdx];
        if (current == -1) break;
    }
    algorithmPath.push_back(nodes[toIdx]);
    
    totalPathWeight = dist[fromIdx][toIdx];
    
    qDebug() << "Camino encontrado con peso total:" << totalPathWeight;
    
    drawPathResult();
}

void MainView::runPrim() {
    qDebug() << "Ejecutando algoritmo de Prim";
    
    const Grafo& grafo = dataManagement->getGrafo();
    
    if (grafo.empty()) {
        QMessageBox::warning(this, "Grafo Vacío", "No hay estaciones en el grafo.");
        return;
    }
    
    std::vector<std::pair<int, int>> mstEdges;
    std::set<int> visited;
    std::priority_queue<std::tuple<float, int, int>,
                       std::vector<std::tuple<float, int, int>>,
                       std::greater<std::tuple<float, int, int>>> pq;
    
    // Empezar desde el primer nodo
    int start = grafo.begin()->first;
    visited.insert(start);
    
    // Agregar todas las aristas del nodo inicial
    auto adjacents = dataManagement->getAdjacentStations(start);
    for (const auto& adj : adjacents) {
        if (!dataManagement->isRouteClosed(start, adj.first)) {
            pq.push({adj.second, start, adj.first});
        }
    }
    
    while (!pq.empty() && visited.size() < grafo.size()) {
        auto [weight, from, to] = pq.top();
        pq.pop();
        
        if (visited.count(to)) continue;
        
        visited.insert(to);
        mstEdges.push_back({from, to});
        
        // Agregar aristas del nuevo nodo
        auto adjacents = dataManagement->getAdjacentStations(to);
        for (const auto& adj : adjacents) {
            if (!visited.count(adj.first) && !dataManagement->isRouteClosed(to, adj.first)) {
                pq.push({adj.second, to, adj.first});
            }
        }
    }
    
    qDebug() << "Árbol de expansión mínima con" << mstEdges.size() << "aristas";
    
    drawTreeResult(mstEdges);
}

void MainView::runKruskal() {
    qDebug() << "Ejecutando algoritmo de Kruskal";
    
    const Grafo& grafo = dataManagement->getGrafo();
    
    if (grafo.empty()) {
        QMessageBox::warning(this, "Grafo Vacío", "No hay estaciones en el grafo.");
        return;
    }
    
    // Recopilar todas las aristas
    std::vector<std::tuple<float, int, int>> edges;
    std::set<std::pair<int, int>> addedEdges;
    
    for (const auto& pair : grafo) {
        int from = pair.first;
        for (const auto& adj : pair.second) {
            int to = adj.first;
            float weight = adj.second;
            
            std::pair<int, int> edge = from < to ? std::make_pair(from, to) : std::make_pair(to, from);
            
            if (addedEdges.count(edge) || dataManagement->isRouteClosed(from, to)) {
                continue;
            }
            
            edges.push_back({weight, from, to});
            addedEdges.insert(edge);
        }
    }
    
    // Ordenar por peso
    std::sort(edges.begin(), edges.end());
    
    // Union-Find
    std::map<int, int> parent;
    for (const auto& pair : grafo) {
        parent[pair.first] = pair.first;
    }
    
    std::function<int(int)> find = [&](int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    };
    
    std::vector<std::pair<int, int>> mstEdges;
    
    for (const auto& [weight, from, to] : edges) {
        int rootFrom = find(from);
        int rootTo = find(to);
        
        if (rootFrom != rootTo) {
            mstEdges.push_back({from, to});
            parent[rootFrom] = rootTo;
        }
    }
    
    qDebug() << "Árbol de expansión mínima con" << mstEdges.size() << "aristas";
    
    drawTreeResult(mstEdges);
}

void MainView::drawPathResult() {
    if (originalPixmap.isNull() || algorithmPath.empty()) {
        return;
    }
    
    QSize availableSize = ui->lblRoutesMap->size();
    routesPixmap = originalPixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    QPainter painter(&routesPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    
    // Dibujar el camino
    for (size_t i = 0; i < algorithmPath.size() - 1; i++) {
        int from = algorithmPath[i];
        int to = algorithmPath[i + 1];
        
        auto coordsFrom = dataManagement->getCoordinates(from);
        auto coordsTo = dataManagement->getCoordinates(to);
        
        float fromX = coordsFrom.first * routesScaleX;
        float fromY = coordsFrom.second * routesScaleY;
        float toX = coordsTo.first * routesScaleX;
        float toY = coordsTo.second * routesScaleY;
        
        // Línea gruesa naranja para el camino
        painter.setPen(QPen(QColor(255, 140, 0), 4));
        painter.drawLine(QPointF(fromX, fromY), QPointF(toX, toY));
    }
    
    // Dibujar estaciones del camino
    for (int stationId : algorithmPath) {
        Station* station = dataManagement->getStation(stationId);
        if (!station) continue;
        
        auto coords = dataManagement->getCoordinates(stationId);
        float scaledX = coords.first * routesScaleX;
        float scaledY = coords.second * routesScaleY;
        
        QColor color = Qt::blue;
        if (stationId == originStationId) {
            color = QColor(0, 180, 0);
        } else if (stationId == destinationStationId) {
            color = QColor(220, 0, 0);
        }
        
        painter.setBrush(QBrush(color));
        painter.setPen(QPen(Qt::white, 3));
        painter.drawEllipse(QPointF(scaledX, scaledY), 10, 10);
        
        // Nombre
        painter.setPen(QPen(Qt::white));
        QFont font = painter.font();
        font.setPointSize(8);
        font.setBold(true);
        painter.setFont(font);
        
        QString stationName = QString::fromStdString(station->name);
        QFontMetrics metrics(font);
        int textWidth = metrics.horizontalAdvance(stationName);
        int textX = scaledX - textWidth / 2;
        int textY = scaledY + 20;
        
        QRect textRect(textX - 2, textY - metrics.ascent() - 1, 
                       textWidth + 4, metrics.height() + 2);
        painter.fillRect(textRect, QColor(0, 0, 0, 150));
        painter.drawText(textX, textY, stationName);
        
        // Mostrar peso total al lado del destino
        if (stationId == destinationStationId) {
            QString weightText = QString("Total: %1 min").arg(QString::number(totalPathWeight, 'f', 1));
            font.setPointSize(10);
            painter.setFont(font);
            QFontMetrics weightMetrics(font);
            int weightWidth = weightMetrics.horizontalAdvance(weightText);
            int weightX = scaledX + 15;
            int weightY = scaledY;
            
            QRect weightRect(weightX - 3, weightY - weightMetrics.ascent() - 2,
                           weightWidth + 6, weightMetrics.height() + 4);
            painter.fillRect(weightRect, QColor(255, 140, 0, 220));
            painter.setPen(QPen(Qt::white));
            painter.drawText(weightX, weightY, weightText);
        }
    }
    
    painter.end();
    ui->lblRoutesMap->setPixmap(routesPixmap);
}

void MainView::drawTreeResult(const std::vector<std::pair<int, int>>& edges) {
    if (originalPixmap.isNull() || edges.empty()) {
        return;
    }
    
    QSize availableSize = ui->lblRoutesMap->size();
    routesPixmap = originalPixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    QPainter painter(&routesPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    
    // Dibujar aristas del árbol
    for (const auto& edge : edges) {
        int from = edge.first;
        int to = edge.second;
        
        auto coordsFrom = dataManagement->getCoordinates(from);
        auto coordsTo = dataManagement->getCoordinates(to);
        
        float fromX = coordsFrom.first * routesScaleX;
        float fromY = coordsFrom.second * routesScaleY;
        float toX = coordsTo.first * routesScaleX;
        float toY = coordsTo.second * routesScaleY;
        
        // Línea verde para el árbol
        painter.setPen(QPen(QColor(0, 180, 0), 4));
        painter.drawLine(QPointF(fromX, fromY), QPointF(toX, toY));
    }
    
    // Recopilar todas las estaciones del árbol
    std::set<int> treeStations;
    for (const auto& edge : edges) {
        treeStations.insert(edge.first);
        treeStations.insert(edge.second);
    }
    
    // Dibujar estaciones
    for (int stationId : treeStations) {
        Station* station = dataManagement->getStation(stationId);
        if (!station) continue;
        
        auto coords = dataManagement->getCoordinates(stationId);
        float scaledX = coords.first * routesScaleX;
        float scaledY = coords.second * routesScaleY;
        
        painter.setBrush(QBrush(QColor(0, 180, 0)));
        painter.setPen(QPen(Qt::white, 3));
        painter.drawEllipse(QPointF(scaledX, scaledY), 10, 10);
        
        // Nombre
        painter.setPen(QPen(Qt::white));
        QFont font = painter.font();
        font.setPointSize(8);
        font.setBold(true);
        painter.setFont(font);
        
        QString stationName = QString::fromStdString(station->name);
        QFontMetrics metrics(font);
        int textWidth = metrics.horizontalAdvance(stationName);
        int textX = scaledX - textWidth / 2;
        int textY = scaledY + 20;
        
        QRect textRect(textX - 2, textY - metrics.ascent() - 1, 
                       textWidth + 4, metrics.height() + 2);
        painter.fillRect(textRect, QColor(0, 0, 0, 150));
        painter.drawText(textX, textY, stationName);
    }
    
    painter.end();
    ui->lblRoutesMap->setPixmap(routesPixmap);
}

void MainView::updateRoutesDisplay() {
    if (isInRoutesTab) {
        if (!algorithmPath.empty()) {
            drawPathResult();
        } else {
            drawRoutesMap();
        }
    }
}

void MainView::closeEvent(QCloseEvent *event) {
    // Guardar automáticamente al cerrar
    saveAllData();
    event->accept();
}

// ==================== SLOTS PARA REPORTES Y PERSISTENCIA ====================

void MainView::on_btnShowTraversal_clicked() {
    BinarySearchTree* bst = dataManagement->getBST();
    
    if (bst->isEmpty()) {
        ui->textEditTraversal->setPlainText("No hay estaciones para mostrar.");
        return;
    }
    
    int selectedIndex = ui->combobox_2->currentIndex();
    QString result;
    
    switch (selectedIndex) {
        case 0: // Pre-orden
            result = QString::fromStdString(bst->getPreOrderString());
            break;
        case 1: // In-orden
            result = QString::fromStdString(bst->getInOrderString());
            break;
        case 2: // Post-orden
            result = QString::fromStdString(bst->getPostOrderString());
            break;
        default:
            result = "Selección inválida";
    }
    
    ui->textEditTraversal->setPlainText(result);
}

void MainView::on_btnGenerateReport_clicked() {
    BinarySearchTree* bst = dataManagement->getBST();
    
    if (bst->isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "No hay estaciones para generar el reporte.");
        return;
    }
    
    // Obtener recorridos
    std::string preOrder = bst->getPreOrderString();
    std::string inOrder = bst->getInOrderString();
    std::string postOrder = bst->getPostOrderString();
    
    // Generar reporte
    bool success = fileManager->generateReport(
        *bst,
        dataManagement->getGrafo(),
        dataManagement->getCoords(),
        dataManagement->getQueryCount(),
        preOrder,
        inOrder,
        postOrder
    );
    
    if (success) {
        QMessageBox::information(this, "Éxito", "Reporte generado exitosamente en saves/reportes.txt");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo generar el reporte.");
    }
}

void MainView::on_btnSaveData_clicked() {
    saveAllData();
    QMessageBox::information(this, "Éxito", "Datos guardados exitosamente en la carpeta saves/");
}

void MainView::on_btnLoadData_clicked() {
    loadAllData();
    updateMapDisplay();
    QMessageBox::information(this, "Éxito", "Datos cargados exitosamente desde la carpeta saves/");
}

void MainView::saveAllData() {
    BinarySearchTree* bst = dataManagement->getBST();
    
    // Guardar estaciones
    fileManager->saveStations(*bst, dataManagement->getCoords());
    
    // Guardar rutas
    fileManager->saveRoutes(dataManagement->getGrafo());
    
    // Guardar cierres
    fileManager->saveClosures(dataManagement->getClosedSegments());
    
    qDebug() << "Todos los datos guardados exitosamente.";
}

void MainView::loadAllData() {
    // Limpiar todos los datos existentes antes de cargar
    dataManagement->clearAllData();
    
    // Obtener referencias mutables
    BinarySearchTree* bst = dataManagement->getBST();
    Coords& coords = dataManagement->getCoordsMutable();
    Grafo& grafo = dataManagement->getGrafoMutable();
    ClosedSegments& closedSegments = dataManagement->getClosedSegmentsMutable();
    
    // Cargar estaciones (también inicializa el grafo)
    fileManager->loadStations(*bst, coords, grafo);
    
    // Cargar rutas
    fileManager->loadRoutes(grafo);
    
    // Cargar cierres
    fileManager->loadClosures(closedSegments);
    
    qDebug() << "Todos los datos cargados exitosamente.";
}


