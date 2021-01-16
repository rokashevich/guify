#pragma once

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowProcess : public MainWindow {
  bool positioning_done_;

 public:
  MainWindowProcess(Cfg* cfg);
  virtual void NumInstancesChanged(int, int) final;
};
