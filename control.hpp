#pragma once

#include <QEvent>
#include <QFrame>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QString>
#include <QWidget>

#include "components/autobutton.hpp"
#include "components/icon.hpp"
class Control : public QFrame {
  Q_OBJECT
  QFrame *workpane_;
  QList<AutoButton *> actionbuttons_;
  void RunStatusScript(QString script_path, Icon *icon, QLabel *label);

 public:
  Control(QString fromDir, QWidget *parent = nullptr);
  ~Control();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);
};
