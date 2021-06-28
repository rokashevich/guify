#pragma once

#include <QEvent>
#include <QFrame>
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>

#include "components/icon.hpp"
class Button : public QFrame {
  Q_OBJECT
  QFrame *workpane_;
  QList<Icon *> icons_;

 public:
  Button(QString fromDir, QWidget *parent = nullptr);
  ~Button();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);
};
