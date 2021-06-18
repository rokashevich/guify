#include "mainwindow.hpp"

#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  this->resize(0, 0);
}

void MainWindow::NumberIndexChanged(int, int) {
  // Для переопределения в наследнике, если надо.
  // По умолчанию никак не реагируем.
}

int MainWindow::AvailableWidth() {
  return this->screen()->availableGeometry().width();
}

int MainWindow::AvailableHeight() {
  return this->screen()->availableGeometry().height();
}
