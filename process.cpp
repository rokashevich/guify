#include "process.hpp"

#include <QObject>
#include <QProcess>
Process::Process(QString command) : QObject() {
  p_ = new QProcess;
  p_->start("sh", {"-c", command});
  connect(p_, &QProcess::finished,
          [this](int exitCode) { emit finished(exitCode); });
}

Process::~Process() {}
