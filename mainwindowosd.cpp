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
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowOsd::MainWindowOsd(Cfg* cfg) : QLabel() {
  this->setText(cfg->Variable().toString());
  this->setStyleSheet("color:white;background:red;padding: 2px 2px 2px 2px;");
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                       Qt::X11BypassWindowManagerHint |
                       Qt::WindowStaysOnTopHint);
}
