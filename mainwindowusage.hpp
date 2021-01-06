#pragma once

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowUsage : public MainWindow {
 public:
  MainWindowUsage(Cfg*);
  virtual void NumInstancesChanged(int, int) final{};
};
