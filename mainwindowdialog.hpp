#pragma once

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowDialog : public MainWindow {
  bool positioning_done_;

 public:
  MainWindowDialog(Cfg* cfg);
  virtual void NumInstancesChanged(int, int) final;
};
