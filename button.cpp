#include "button.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
Button::Button(QString fromDir, QFrame *parent) : QFrame(parent) {
  QHBoxLayout *layout = new QHBoxLayout();
  this->setLayout(layout);

  QDir dir{fromDir};
  if (!dir.exists()) {
    layout->addWidget(new QLabel("Bad dir `" + fromDir + "`"));
  } else {
    const QString dirFullPath = dir.absolutePath();
    QFile qss = QFile(dir.filePath("style.qss"));
    if (qss.exists()) {
      qss.open(QFile::ReadOnly);
      this->setStyleSheet(qss.readAll());
    }
    layout->addWidget(new QLabel(qss.fileName()));
  }
}
Button::~Button() {}
