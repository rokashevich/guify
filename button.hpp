#pragma once

#include <QEvent>
#include <QFrame>
#include <QObject>
#include <QString>

class Button : public QFrame {
  Q_OBJECT
 public:
  Button(QString fromDir, QFrame *parent = nullptr);
  ~Button();
};
