#include "mainwindowdialog.hpp"

#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
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
  QVBoxLayout* vbl = new QVBoxLayout;
  for (auto& s : cfg->Sentenses()) {
    QGroupBox* gb = new QGroupBox;
    QHBoxLayout* hbl = new QHBoxLayout;
    QString type = s.at(0);
    QString title = s.at(1);

    QStringList values(s.begin() + 2, s.end());
    hbl->addWidget(new QLabel(title));
    for (auto& ss : values) {
      if (type == "-I")
        hbl->addWidget(new QLineEdit(ss));
      else if (type == "-R")
        hbl->addWidget(new QRadioButton(ss));
      else if (type == "-C")
        hbl->addWidget(new QCheckBox(ss));
      else if (type == "-D") {
        qDebug() << "DDD";
        QPushButton* b = new QPushButton(ss);
        hbl->addWidget(b);
        connect(b, &QPushButton::clicked,
                [b]() { b->setText(QFileDialog::getExistingDirectory()); });
      }
    }
    gb->setLayout(hbl);
    vbl->addWidget(gb);
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
