#include "control.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QEvent>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QSvgWidget>

#include "components/autobutton.hpp"
#include "components/icon.hpp"
Control::Control(QString fromDir, QWidget *parent) : QFrame(parent) {
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
      QDirIterator it(subdirPath, QDir::Dirs | QDir::NoDotAndDotDot);
      if (!it.hasNext()) {
        layout->addWidget(new QLabel("No autobuttons subfolders"));
        continue;
      } else {
        while (it.hasNext()) {
          const auto autobutton_path = it.next();
          actionbuttons_.append(new AutoButton(autobutton_path));
        }
      }

      workpane_ = new QFrame();
      workpane_->hide();
      workpane_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint |
                                Qt::X11BypassWindowManagerHint);
      workpane_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint |
                                Qt::X11BypassWindowManagerHint);
      workpane_->setMaximumSize(QSize(0, 0));

      QHBoxLayout *layout = new QHBoxLayout();  // для автокнопок
      for (auto &ab : actionbuttons_) {
        layout->addWidget(ab);
      }
      workpane_->setLayout(layout);
    } else {
      const QString iconPath = QDir(subdirPath).filePath("icon.svg");
      const auto status_script_path{QDir(subdirPath).filePath("status.sh")};
      if (!QFile(iconPath).exists() || !QFile(status_script_path).exists()) {
        layout->addWidget(new QLabel("No icon.svg or status.sh"));
        continue;
      }
      layout->addWidget(new Icon(iconPath));
      QLabel *label = new QLabel("");
      label->setMaximumSize(0, 0);
      layout->addWidget(label);
    }
  }
}
Control::~Control() {}

void Control::mousePressEvent(QMouseEvent *event) {
  Q_UNUSED(event)
  if (!actionbuttons_.size()) return;
  if (workpane_->isVisible()) {
    workpane_->hide();
  } else {
    const QPoint globalPos = this->mapToGlobal(QPoint{});
    const int panelX = globalPos.x();
    const int panelY = globalPos.y();

    const int panelW = width();
    const int panelH = height();
    const int workpaneW = workpane_->width();
    const int workpaneX = panelX + panelW - workpaneW;
    const int workpaneY = panelY + panelH;
    workpane_->show();
    workpane_->move(panelX, workpaneY);
  }
}
