#pragma once

#include <QLabel>

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowOsd : public QLabel {
 public:
  MainWindowOsd(Cfg& cfg);
};
