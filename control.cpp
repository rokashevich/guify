#include "control.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QEvent>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLabel>
#include <QPixmap>
#include <QPoint>
#include <QProcess>
#include <QSizePolicy>
#include <QString>

#include "components/autobutton.hpp"
#include "components/icon.hpp"

static auto IndicatorSandboxSetup(const QString sandbox) {
  struct {
    QString icon_path;
    QString status_script_path;
    QString verdict{};
  } s;
  s.icon_path = QDir(sandbox).filePath("icon.svg");
  s.status_script_path = QDir(sandbox).filePath("status.sh");
  const bool icon_exists = QFile(s.icon_path).exists();
  const bool script_exists = QFile(s.status_script_path).exists();
  if (!icon_exists && !script_exists) {
    s.verdict = "No icon.svg or status.sh";
  } else if (!icon_exists) {
    s.verdict = "No icon.svg";
  } else if (!script_exists) {
    s.verdict = "No icon.svg";
  }
  return s;
}

Control::Control(QString fromDir, QWidget *parent) : QFrame(parent) {
  QHBoxLayout *layout = new QHBoxLayout();
  this->setLayout(layout);

  QDir dir{fromDir};
  if (!dir.exists()) {
    layout->addWidget(new QLabel("Bad dir `" + fromDir + "`"));
    return;
  }
  const QString dirFullPath = dir.absolutePath();

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

      //      workpane_ = new QFrame();
      workpane_.hide();
      workpane_.setWindowFlags(Qt::Widget | Qt::FramelessWindowHint |
                               Qt::X11BypassWindowManagerHint);
      workpane_.setWindowFlags(Qt::Widget | Qt::FramelessWindowHint |
                               Qt::X11BypassWindowManagerHint);
      //      workpane_.setMaximumSize(QSize(0, 0));

      QHBoxLayout *layout = new QHBoxLayout();  // для автокнопок
      for (auto &ab : actionbuttons_) {
        layout->addWidget(ab);
      }
      workpane_.setLayout(layout);
    } else {
      auto [icon_path, status_script_path, verdict] =
          IndicatorSandboxSetup(subdirPath);
      if (!verdict.isEmpty()) {
        layout->addWidget(new QLabel(verdict));
        continue;
      }

      Icon *icon = new Icon(icon_path);
      QLabel *label = new QLabel;
      RunStatusScript(status_script_path, icon, label);

      layout->addWidget(icon);
      layout->addWidget(label);
      label->setMaximumWidth(0);
      label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }
  }

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  // Применяем  файл со стилями, если есть.
  QFile qss = QFile(dir.filePath("style.qss"));
  if (qss.exists()) {
    qss.open(QFile::ReadOnly);
    style_ = qss.readAll();
    workpane_.setStyleSheet(style_);
    ApplyStyleReleased();
  }
}
Control::~Control() {}

void Control::RunStatusScript(QString path, Icon *icon, QLabel *label) {
  QProcess *p{new QProcess};
  connect(p, &QProcess::readyReadStandardOutput, [icon, label, p]() {
    while (p->canReadLine()) {
      const QByteArray b{p->readLine()};
      QJsonParseError e;
      QJsonDocument j{QJsonDocument::fromJson(b, &e)};
      if (e.error != QJsonParseError::NoError) {
        label->setText(e.errorString());
      } else {
        const QJsonObject o{j.object()};
        if (o.contains("label")) {
          const QString text{o.value("label").toString()};
          label->setText(text);
        }
        if (o.contains("color")) {
          const QString color{o.value("color").toString()};
          icon->setColor(color);
        }
      }
      const QString text{label->text()};
      const int width{label->fontMetrics().boundingRect(text).width()};
      label->setFixedWidth(width);
    }
  });
  p->start("/bin/bash", {path});
}

void Control::mousePressEvent(QMouseEvent *event) {
  Q_UNUSED(event)
  if (!actionbuttons_.size()) return;
  if (workpane_.isVisible()) {
    workpane_.hide();
    ApplyStyleReleased();
  } else {
    const QPoint globalPos = this->mapToGlobal(QPoint{});
    const int panelX = globalPos.x();
    const int panelY = globalPos.y();

    const int panelW = width();
    const int panelH = height();
    const int workpaneW = workpane_.width();
    const int workpaneX = panelX + panelW - workpaneW;
    const int workpaneY = panelY + panelH;
    workpane_.show();
    workpane_.move(panelX - workpaneW + panelW, workpaneY);
    ApplyStylePressed();
  }
}

void Control::ApplyStyleReleased() {
  setObjectName("released");
  setStyleSheet(style_);
}
void Control::ApplyStylePressed() {
  setObjectName("pressed");
  setStyleSheet(style_);
}
