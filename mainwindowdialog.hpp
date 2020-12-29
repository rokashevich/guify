#ifndef MAINWINDOWDIALOG_H
#define MAINWINDOWDIALOG_H

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowDialog : public MainWindow {
  bool positioning_done_;

 public:
  MainWindowDialog(Cfg* cfg);
  virtual void NumInstancesChanged(int, int) final;
};

#endif  // MAINWINDOWDIALOG_H
