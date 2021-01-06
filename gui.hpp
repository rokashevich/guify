#pragma once
#include <QApplication>
#include <QMainWindow>

#include "cfg.hpp"
#include "mainwindow.hpp"

class Gui {
  QApplication* application_;
  MainWindow* mainwindow_;

 public:
  Gui(Cfg*);
  ~Gui();
  int Run();
  void NumInstancesChanged(int, int);
};
