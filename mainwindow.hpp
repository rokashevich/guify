#pragma once
#include <QDebug>
#include <QMainWindow>
#include <QObject>
#include <QScreen>
class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget* parent = nullptr);
  virtual void NumberIndexChanged(int, int);
  int AvailableWidth();
  int AvailableHeight();
};
