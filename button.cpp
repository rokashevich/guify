#include "button.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QSvgWidget>

#include "icon.hpp"
Button::Button(QString fromDir, QWidget *parent) : QFrame(parent) {
  QHBoxLayout *layout = new QHBoxLayout();
  this->setLayout(layout);

  QDir dir{fromDir};
  if (!dir.exists()) {
    layout->addWidget(new QLabel("Bad dir `" + fromDir + "`"));
    return;
  }
  const QString dirFullPath = dir.absolutePath();

  // Применяем  файл со стилями, если есть.
  QFile qss = QFile(dir.filePath("style.qss"));
  if (qss.exists()) {
    qss.open(QFile::ReadOnly);
    this->setStyleSheet(qss.readAll());
  }

  QDirIterator it(dirFullPath, QDir::Dirs | QDir::NoDotAndDotDot);
  while (it.hasNext()) {
    const QString subdirPath = it.next();
    QString subdirName = QFileInfo(subdirPath).fileName();
    if (subdirName == "autobuttons") {
      // TODO: выпадающая панель
    } else {
      const QString iconPath = QDir(subdirPath).filePath("icon.svg");
      QFile icon = QFile(iconPath);
      if (icon.exists()) {
        layout->addWidget(new Icon(iconPath));
      }
      layout->addWidget(new QLabel(""));
    }
  }
}
Button::~Button() {}
