#pragma once

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowProcess : public MainWindow {
 public:
  MainWindowProcess(Cfg* cfg);
  void NumInstancesChanged(int, int) override;
};
