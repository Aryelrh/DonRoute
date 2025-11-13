//
// Created by aryel on 11/12/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainView.h" resolved

#include "../header/mainview.h"
#include "../header/DataManagement.h"
#include "ui_mainview.h"
#include <QPixmap>
#include <QDebug>
#include <QResizeEvent>
#include <QShowEvent>
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
MainView::MainView(QWidget *parent) : QWidget(parent), ui(new Ui::MainView), isInFullscreen(false) {
    ui->setupUi(this);
    
    dataManagement = new DataManagement();
    
    normalWindowSize = QSize(980, 641);  // Tamaño inicial de la ventana
    
    // Configurar el QLabel para que escale el contenido
    ui->lblInteractiveMap->setScaledContents(false);
    ui->lblInteractiveMap->setAlignment(Qt::AlignCenter);
    
    loadMapImage();
}

MainView::~MainView() {
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
        // Usar un timer para asegurarnos de que el label tiene su tamaño final
        QTimer::singleShot(0, this, [this]() {
            resizeLabel();
        });
    }
}

void MainView::mouseDoubleClickEvent(QMouseEvent *event) {
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
    
    // Dibujar cada estación usando el map de coords
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
