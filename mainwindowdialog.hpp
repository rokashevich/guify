#pragma once

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowDialog : public MainWindow {
 public:
  MainWindowDialog(Cfg& cfg);
};
