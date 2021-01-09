#pragma once

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowUsage : public MainWindow {
 public:
  MainWindowUsage();
  virtual void NumInstancesChanged(int, int) final{};
};
