#include "mainwindowdialog.hpp"

#include <QCheckBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowDialog::MainWindowDialog(Cfg* cfg)
    : MainWindow(), positioning_done_(false) {
  QVBoxLayout* vbl = new QVBoxLayout;
  for (auto s : cfg->Sentenses()) {
    QHBoxLayout* hbl = new QHBoxLayout;
    QString type = s.at(0);
    QString title = s.at(1);

    QVector<QString> values;  // = QVector<QString>::fromStdVector(s);
    hbl->addWidget(new QLabel(title));
    for (auto ss : values) {
      hbl->addWidget(new QLabel(ss));
    }
    vbl->addLayout(hbl);
  }
  QWidget* w = new QWidget;
  w->setLayout(vbl);
  this->setCentralWidget(w);
}

void MainWindowDialog::NumInstancesChanged(int number, int index) {
  Q_UNUSED(number);
  Q_UNUSED(index);
  if (positioning_done_) return;
  positioning_done_ = true;
}
