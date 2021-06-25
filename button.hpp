#pragma once

#include <QEvent>
#include <QFrame>
#include <QObject>
#include <QString>

class Button : public QFrame {
  Q_OBJECT
  QFrame *workpane_;

 public:
  Button(QString fromDir, QWidget *parent = nullptr);
  ~Button();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);
};
