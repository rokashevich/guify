#pragma once

#include <QLabel>
#include <QObject>

#include "cfg.hpp"
#include "mainwindow.hpp"

class MainWindowOsd : public QLabel {
  Q_OBJECT
 public:
  MainWindowOsd(Cfg& cfg);
};
