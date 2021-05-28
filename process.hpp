#pragma once
#include <QObject>
#include <QProcess>
#include <QString>
class Process : public QObject {
  Q_OBJECT
  QProcess* p_;
 signals:
  void finished(int exitCode);

 public:
  Process(QString command);
  ~Process();
};
