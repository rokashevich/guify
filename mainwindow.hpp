#pragma once
#include <QDebug>
#include <QMainWindow>
class MainWindow : public QMainWindow {
  Q_OBJECT

  int available_width_;
  int available_height_;

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  virtual void NumberIndexChanged(int, int);
};
