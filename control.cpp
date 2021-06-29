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
      QHBoxLayout *layout = new QHBoxLayout();
      QDirIterator it(subdirPath, QDir::Dirs | QDir::NoDotAndDotDot);
      while (it.hasNext()) {
        const QString subdirPath = it.next();
        const QString iconPath = QDir(subdirPath).filePath("icon.svg");
        if (!QFile(iconPath).exists()) {
          QLabel *label = new QLabel("No icon `" + it.fileName() + "`");
          layout->addWidget(label);
          continue;
        } else {
          const QList<QStringList> possible_variants{{"detach.sh"},
                                                     {"start.sh", "stop.sh"}};
          QList<QStringList> variants_found;
          for (const QStringList &variant : possible_variants) {
            const int num_scripts_in_variant = variant.size();
            int num_scripts_found{};
            for (const QString &script_name : variant) {
              const QString script_path =
                  QDir(subdirPath).filePath(script_name);
              if (!QFile(script_path).exists()) {
                break;
              }
              ++num_scripts_found;
            }
            if (num_scripts_found == num_scripts_in_variant) {
              variants_found.append(variant);
            } else if (num_scripts_found > 0) {
              variants_found.clear();
              break;
            }
          }
          if (variants_found.size() != 1) {
            QLabel *label = new QLabel("Bad scripts `" + it.fileName() + "`");
            layout->addWidget(label);
            continue;  // переходим к следующей иконке
          }
          Icon *icon = new Icon(iconPath);
          layout->addWidget(icon);
          icons_.append(icon);
        }
      }
      workpane_ = new QFrame();
      workpane_->hide();
      workpane_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint |
                                Qt::X11BypassWindowManagerHint);
      workpane_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint |
                                Qt::X11BypassWindowManagerHint);
      workpane_->setMaximumSize(QSize(0, 0));
      workpane_->setLayout(layout);
    } else {
      const QString iconPath = QDir(subdirPath).filePath("icon.svg");
      if (QFile(iconPath).exists()) {
        layout->addWidget(new Icon(iconPath));
      }
      QLabel *label = new QLabel("");
      label->setMaximumSize(0, 0);
      layout->addWidget(label);
    }
  }
}
Control::~Control() {}

void Control::mousePressEvent(QMouseEvent *event) {
  Q_UNUSED(event)
  if (!icons_.size()) return;
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
