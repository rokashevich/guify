#pragma once

#include <QByteArray>
#include <QDebug>
#include <QDomDocument>
#include <QEvent>
#include <QFile>
#include <QIcon>
#include <QObject>
#include <QPainter>
#include <QSize>
#include <QString>
#include <QSvgRenderer>
#include <QSvgWidget>
#include <QWidget>
#include <QtGlobal>
class Icon : public QSvgWidget {
  Q_OBJECT
  QString color_;
  QDomDocument dom_;

 public:
  Icon(QString file, QWidget *parent = nullptr) : QSvgWidget(parent) {
    QFile f{file};
    QByteArray ba;
    f.open(QIODevice::ReadOnly);
    ba = f.readAll();
    f.close();
    dom_.setContent(ba);
    this->load(dom_.toByteArray());

    // Обязательно! Выставляем размер QSvgWidget-у, иначе он размера 0х0!
    setMinimumSize(sizeHint());
  }

  void setColor(QString color) {
    color_ = color;
    QDomElement root = dom_.documentElement();
    QDomElement el = root.firstChildElement("path");
    el.setAttribute("fill", color_);
    this->load(dom_.toByteArray());
  }

  QString color() { return color_; }
};
