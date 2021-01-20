#include "mainwindowprocess.hpp"

#include <QApplication>
#include <QChar>
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowProcess::MainWindowProcess(Cfg* cfg) : MainWindow() {
  const auto setup = static_cast<Cfg::Process*>(cfg->Setup());
  setWindowTitle(setup->binary);

  QProcessEnvironment env;
  for (const auto& e : qAsConst(setup->environment))
    env.insert(e.first, e.second);
  QProcess* p = new QProcess;
  p->setProcessEnvironment(env);
  p->setProgram(setup->binary);
  p->setArguments(setup->arguments);
  p->start();

  QPlainTextEdit* te = new QPlainTextEdit;
  te->setMaximumBlockCount(1024);
  connect(p, &QProcess::readyReadStandardOutput, [te, p]() {
    const QString line = p->readAllStandardOutput().trimmed();
    te->appendPlainText(line);
    fprintf(stdout, "%s\n", line.toStdString().c_str());
  });
  connect(p, &QProcess::readyReadStandardError, [te, p]() {
    const QString line = p->readAllStandardError().trimmed();
    te->appendHtml("<font color='red'>" + line + "</font>");
    fprintf(stderr, "%s\n", line.toStdString().c_str());
  });
  this->setCentralWidget(te);

  connect(p, &QProcess::finished, []() { QApplication::quit(); });
}

void MainWindowProcess::NumInstancesChanged(int number, int index) {
  int x = 100;
  int y = 100;
  int w = AvailableWidth() - 200;
  int h = AvailableHeight() - 200;
  if (number > 1) {
    w /= 2;
    if (number % 2) {  // четное кол-во окон
      h /= number / 2;
    } else {  // нечётное кол-во окон
      h = number == index ? h / (number + 1) / 2 : h / number / 2;
    }
    if (index > number / 2) {
      x = x + w;
    }
    y += index * h;
  }
  // setGeometry(x, y, w, h);
}
