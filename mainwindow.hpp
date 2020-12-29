#pragma once
#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

  int available_width_;
  int available_height_;

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  virtual void NumInstancesChanged(int, int) = 0;
  int AvailableWidth();
  int AvailableHeight();
 signals:
};
