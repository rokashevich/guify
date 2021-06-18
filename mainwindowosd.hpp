#pragma once

#include <QLabel>
#include <QObject>

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowOsd : public MainWindow {
  Q_OBJECT
  QLabel* label_;

 public:
  MainWindowOsd(Cfg& cfg);
};
