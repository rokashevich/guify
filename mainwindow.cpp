#include "mainwindow.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {}

void MainWindow::NumInstancesChanged(int, int) {
  // Для переопределения в наследнике, если надо.
  // По умолчанию никак не реагируем.
}

int MainWindow::AvailableWidth() {
  return QGuiApplication::primaryScreen()->availableSize().width();
}
int MainWindow::AvailableHeight() {
  return QGuiApplication::primaryScreen()->availableSize().height();
}
