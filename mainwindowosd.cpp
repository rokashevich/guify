#include "mainwindowosd.hpp"

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

MainWindowOsd::MainWindowOsd(Cfg& cfg) : MainWindow() {
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                       Qt::X11BypassWindowManagerHint |
                       Qt::WindowStaysOnTopHint);
  label_ = new QLabel(cfg.Settings().first().first());
  label_->setStyleSheet("color:white;background:red;padding: 2px 2px 2px 2px;");
  this->setCentralWidget(label_);
  connect(&cfg, &Cfg::processFinished, [this](int) {
    this->close();
    emit this->closed();
  });
}
