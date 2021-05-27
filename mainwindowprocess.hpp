#pragma once

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowProcess : public MainWindow {
 public:
  MainWindowProcess(Cfg& cfg);
  void NumberIndexChanged(int, int) override;
};
