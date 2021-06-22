#include "button.hpp"

#include <QDebug>
#include <QDirIterator>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
Button::Button(QString fromDir, QFrame *parent) : QFrame(parent) {
  QLabel *lb = new QLabel(fromDir);
  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(lb);
  this->setLayout(layout);
}
Button::~Button() {}
