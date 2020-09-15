#include "mainwindow.h"

MainWindow::MainWindow(const Cfg &cfg, QWidget *parent) : QMainWindow(parent) {
  setWindowFlags(Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{
  
}

