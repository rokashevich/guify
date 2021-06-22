#include "mainwindowpanel.hpp"

#include <QChar>
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowPanel::MainWindowPanel(Cfg& cfg) : MainWindow() {
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                       Qt::X11BypassWindowManagerHint |
                       Qt::WindowStaysOnTopHint);
  QString text;
  qDebug() << cfg.Variable();
  for (auto& variant : cfg.Variable().toList()) {
    const Panel::Entry entry = variant.value<Panel::Entry>();
    if (entry.type == Panel::Type::kButton) {
      text += entry.data.toString();
    }
  }
  label_ = new QLabel{text};
  label_->setStyleSheet("color:white;background:red;padding: 2px 2px 2px 2px;");
  this->setCentralWidget(label_);
}
