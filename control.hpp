#pragma once

#include <QEvent>
#include <QFrame>
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>

#include "components/icon.hpp"
class Control : public QFrame {
  Q_OBJECT
  QFrame *workpane_;
  QList<Icon *> icons_;

 public:
  Control(QString fromDir, QWidget *parent = nullptr);
  ~Control();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);
};
