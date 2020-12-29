#include "mainwindow.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  QScreen* screen = QGuiApplication::primaryScreen();

  // this->setGeometry(100, 100, 500, 500);
}
