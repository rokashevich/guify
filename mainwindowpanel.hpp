#pragma once

#include <QLabel>
#include <QObject>

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowPanel : public MainWindow {
  Q_OBJECT
  QLabel* label_;

 public:
  MainWindowPanel(Cfg& cfg);
};
