#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "cfg.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  MainWindow(const Cfg &cfg, QWidget *parent = nullptr);
  ~MainWindow();
};
#endif // MAINWINDOW_H
