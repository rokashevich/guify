#pragma once

// Назначение класса - распарсить аргументы командной строки.
// Помимо проверки введённых параметров на правильность,
// производится поиск:
//   - самого длинного названия панели (левый столбик);
//   - самый длинный вариант;
//   - наибольшее кол-во вариантов в панели.
// Эти поля будут нужны для красивой компоновки виджетов в классе Xxdialog.

#include <QList>
#include <QPair>
#include <QSet>
#include <QStringList>
#include <QVector>

class Cfg {
 public:
  enum class Mode { kDialog, kProcess, kProgressBar, kOSD, kMenu };

  enum class ModeDialog { kInput, kRadio, kCheck, kDir, kFile };
  struct DialogEntry {
    ModeDialog type;
    QString title;
    QStringList params;
  };
  struct Dialog {
    QVector<DialogEntry> params;
  };
  struct Process {
    QSet<QPair<QString, QString>> environment;
    QString binary;
    QStringList arguments;
  };

 private:
  int argc_;
  char** argv_;
  QStringList mode_params_;
  Mode mode_;
  void* setup_;
  QString title_;
  QString config_error_;

 public:
  Cfg(int argc, char** argv);
  ~Cfg();
  void* Setup() { return setup_; }
  Mode mode() { return mode_; }
  int& Argc() { return argc_; }
  char** Argv() { return argv_; }
  void* ModeDialog();
  void* ModeProcess();
  QString Title() { return title_; }
  QString ConfigError() { return config_error_; }
};
