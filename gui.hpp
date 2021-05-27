#pragma once
#include <QApplication>
#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include "cfg.hpp"
#include "mainwindow.hpp"

class Gui : public QObject {
  Q_OBJECT

  QWidget* mainwindow_;

 public:
  Gui(Cfg*);
  ~Gui(){};
  int Run();
  void SwarmCallback(int, int);
 signals:
  void NumberIndexChanged(int a, int b);
};
