#include "mainwindowdialog.hpp"

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
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowDialog::MainWindowDialog(Cfg* cfg)
    : MainWindow(), positioning_done_(false) {
  QGridLayout* gl = new QGridLayout;
  QVector<Cfg::DialogEntry>* setup =
      static_cast<QVector<Cfg::DialogEntry>*>(cfg->Setup());
  int row = 0;
  for (const auto& a : *setup) {
    const auto type = a.type;
    const auto title = a.title;
    const auto params = a.params;
    QHBoxLayout* hbl = new QHBoxLayout;
    switch (type) {
      case Cfg::SetupDialog::kInput:
        hbl->addWidget(new QLineEdit(params.at(0)));
        break;
      case Cfg::SetupDialog::kRadio:
        for (const auto& p : params) hbl->addWidget(new QRadioButton(p));
        break;
      case Cfg::SetupDialog::kCheck:
        for (const auto& p : params) hbl->addWidget(new QCheckBox(p));
        break;
      case Cfg::SetupDialog::kDir: {
        QPushButton* b = new QPushButton(params.at(0));
        connect(b, &QPushButton::clicked,
                [b]() { b->setText(QFileDialog::getExistingDirectory()); });
        hbl->addWidget(b);
      } break;
      default:
        break;
    }
    QGroupBox* gb = new QGroupBox;
    gb->setLayout(hbl);
    gl->addWidget(new QLabel(title), row, 0);
    gl->addWidget(gb, row, 1);
    ++row;
  }
  QWidget* w = new QWidget;
  w->setLayout(gl);
  this->setCentralWidget(w);
}

void MainWindowDialog::NumInstancesChanged(int number, int index) {
  Q_UNUSED(number);
  Q_UNUSED(index);
  if (positioning_done_) return;
  positioning_done_ = true;
}
