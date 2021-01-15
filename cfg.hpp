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
#include <QStringList>
#include <QVector>

class Cfg {
 public:
  enum class Mode { kUsage, kDialog, kProcess, kProgressBar, kOSD, kMenu };

  enum class ModeDialog { kInput, kRadio, kCheck, kDir };
  struct DialogEntry {
    ModeDialog type;
    QString title;
    QStringList params;
  };
  struct Dialog {
    QString title;
    QVector<DialogEntry> params;
  };

 private:
  int argc_;
  char** argv_;
  const QStringList mode_params_;
  Mode mode_;
  void* setup_;

 public:
  Cfg(int argc, char** argv);
  ~Cfg();
  auto& Setup() { return setup_; }
  Mode mode() { return mode_; }
  int& Argc() { return argc_; }
  char** Argv() { return argv_; }
  void* ModeDialog();
};
