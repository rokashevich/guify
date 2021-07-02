#pragma once

#include <QByteArray>
#include <QDebug>
#include <QDomDocument>
#include <QEvent>
#include <QFile>
#include <QIcon>
#include <QObject>
#include <QPainter>
#include <QRegularExpression>
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
  QSize size_;

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
    const QDomElement root = dom_.documentElement();
    const QString stylesheet = root.attribute("style");
    const QRegularExpression re("(?<width>[0-9]*)px");
    const QRegularExpressionMatch match = re.match(stylesheet);
    const int size = match.hasMatch() ? match.captured("width").toInt() : 24;
    size_ = QSize{size, size};
    setMinimumSize(size_);
  }

  void setColor(QString color) {
    color_ = color;
    QDomElement root = dom_.documentElement();
    QDomElement el = root.firstChildElement("path");
    el.setAttribute("fill", color_);
    this->load(dom_.toByteArray());
  }

  QString color() { return color_; }

  QIcon Qicon() {
    QSvgRenderer renderer;
    renderer.load(dom_.toByteArray());
    QPixmap pm(size_);
    pm.fill("transparent");
    QPainter painter(&pm);
    renderer.render(&painter, pm.rect());
    return QIcon(pm);
  }
};
