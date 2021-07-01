#pragma once

#include <QEvent>
#include <QFrame>
#include <QList>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QString>
#include <QWidget>

#include "components/autobutton.hpp"
class Control : public QFrame {
  Q_OBJECT
  QFrame *workpane_;
  QList<AutoButton *> actionbuttons_;

 public:
  Control(QString fromDir, QWidget *parent = nullptr);
  ~Control();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);
};
