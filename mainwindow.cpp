#include "mainwindow.hpp"

#include <QDebug>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {}

void MainWindow::NumberIndexChanged(int, int) {
  // Для переопределения в наследнике, если надо.
  // По умолчанию никак не реагируем.
}
