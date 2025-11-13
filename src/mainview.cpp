//
// Created by aryel on 11/12/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainView.h" resolved

#include "../header/mainview.h"
#include "ui_mainview.h"
#include <QPixmap>
#include <QDebug>
#include <QResizeEvent>
#include <QCoreApplication>
#include <QSizePolicy>

MainView::MainView(QWidget *parent) : QWidget(parent), ui(new Ui::MainView) {
    ui->setupUi(this);
    
    // Cargar el mapa en el label
    loadMapImage();
}

MainView::~MainView() {
    delete ui;
}

void MainView::loadMapImage() {
    // Ruta del archivo de imagen
    QString imagePath = ":/resources/styles/map.jpeg";
    
    // Cargar la imagen original
    QPixmap pixmap(imagePath);
    
    if (pixmap.isNull()) {
        qWarning() << "Error: No se pudo cargar la imagen del mapa desde:" << imagePath;
        return;
    }
    
    // Guardar la imagen original para redimensionarla cuando sea necesario
    originalPixmap = pixmap;
    
    // Mostrar la imagen escalada inicialmente
    resizeLabel();
    
    qDebug() << "Imagen del mapa cargada:" << pixmap.width() << "x" << pixmap.height();
}

void MainView::resizeLabel() {
    if (originalPixmap.isNull()) {
        return;
    }
    
    // Calcular tamaño máximo basado en el espacio disponible
    QSize availableSize = ui->lblInteractiveMap->size();
    
    // Si no hay espacio disponible aún, usar el tamaño mínimo por defecto
    if (availableSize.width() <= 0 || availableSize.height() <= 0) {
        availableSize = QSize(800, 500);
    }
    
    qDebug() << "Available space:" << availableSize;
    qDebug() << "Original pixmap:" << originalPixmap.size();
    
    // Escalar la imagen manteniendo relación de aspecto
    QPixmap scaledPixmap = originalPixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    qDebug() << "Scaled to:" << scaledPixmap.size();
    
    // Mostrar en el label
    ui->lblInteractiveMap->setPixmap(scaledPixmap);
    ui->lblInteractiveMap->setAlignment(Qt::AlignCenter);
}

void MainView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    
    qDebug() << "resizeEvent triggered, new size:" << event->size();
    
    // Llamar a resizeLabel inmediatamente y también después para asegurar sincronización
    resizeLabel();
    
    // Si es un cambio grande de tamaño (como fullscreen), procesar eventos pendientes
    if (event->oldSize().width() > 0) {
        QCoreApplication::processEvents();
    }
}