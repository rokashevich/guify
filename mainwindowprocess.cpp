#include "mainwindowprocess.hpp"

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
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowProcess::MainWindowProcess(Cfg* cfg)
    : MainWindow(), positioning_done_(false) {
  const auto setup = static_cast<Cfg::Process*>(cfg->Setup());
  setWindowTitle(setup->binary);

  QString s;
  for (auto env : setup->environment)
    s += "env: " + env.first + "=" + env.second + "\n";
  for (auto arg : setup->arguments) s += "arg: " + arg + "\n";

  QTextEdit* te = new QTextEdit;
  te->setText(s);
  this->setCentralWidget(te);
}

void MainWindowProcess::NumInstancesChanged(int number, int index) {
  Q_UNUSED(number);
  Q_UNUSED(index);
  if (positioning_done_) return;
  positioning_done_ = true;
}
