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

// Обёртка над QFrame чтобы можно было применять стили из style.qss.
class Workpane : public QFrame {
  Q_OBJECT  // Обязательно, чтобы стили работали!
};

class Control : public QFrame {
  Q_OBJECT
  QByteArray style_;
  Workpane workpane_;
  QList<AutoButton *> actionbuttons_;
  int width_{0};
  void RunStatusScript(QString script_path, Icon *icon, QLabel *label);
  void ApplyStyleReleased();
  void ApplyStylePressed();

 public:
  Control(QString fromDir, QWidget *parent = nullptr);
  ~Control();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);
};
