#include "mainwindowprocess.hpp"

#include <QApplication>
#include <QChar>
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QRadioButton>
#include <QRect>
#include <QScreen>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowProcess::MainWindowProcess(Cfg& cfg) : MainWindow() {
  //  const auto setup = static_cast<Cfg::Process*>(cfg->Variable());
  //  setWindowTitle(setup->binary);

  //  QProcessEnvironment env;
  //  for (const auto& e : qAsConst(setup->environment))
  //    env.insert(e.first, e.second);
  //  QProcess* p = new QProcess;
  //  p->setProcessEnvironment(env);
  //  p->setProgram(setup->binary);
  //  p->setArguments(setup->arguments);
  //  p->start();

  //  QPlainTextEdit* te = new QPlainTextEdit;
  //  te->setMaximumBlockCount(1024);
  //  connect(p, &QProcess::readyReadStandardOutput, [te, p]() {
  //    const QString line = p->readAllStandardOutput().trimmed();
  //    te->appendPlainText(line);
  //    fprintf(stdout, "%s\n", line.toStdString().c_str());
  //  });
  //  connect(p, &QProcess::readyReadStandardError, [te, p]() {
  //    const QString line = p->readAllStandardError().trimmed();
  //    te->appendHtml("<font color='red'>" + line + "</font>");
  //    fprintf(stderr, "%s\n", line.toStdString().c_str());
  //  });
  //  this->setCentralWidget(te);

  //  connect(p,
  //    static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>
  //      (&QProcess::finished), [](){QGuiApplication::quit();});
}

void MainWindowProcess::NumberIndexChanged(int number, int index) {
  //  qDebug() << "number:" << number << "index:" << index;
  const QRect geom = QGuiApplication::primaryScreen()->availableGeometry();
  int x = geom.x();
  int y = geom.y();
  int w = geom.width();
  int h = geom.height();
  if (number > 1) {
    w /= 2;
    const int half = number / 2;
    const int column_rows =
        number % 2 == 0 ? half : index < half ? half : half + 1;

    h /= column_rows;
    x = index < half ?: x + w;
    y += index < half ? index * h : (index - half) * h;
    //    qDebug() << "column_rows:" << column_rows;
  }
  setGeometry(x, y, w, h);
}
