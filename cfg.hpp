#pragma once

// Назначение класса - распарсить аргументы командной строки.
// Помимо проверки введённых параметров на правильность,
// производится поиск:
//   - самого длинного названия панели (левый столбик);
//   - самый длинный вариант;
//   - наибольшее кол-во вариантов в панели.
// Эти поля будут нужны для красивой компоновки виджетов в классе Xxdialog.

#include <QList>
#include <QStringList>
#include <iostream>
#include <vector>

class Cfg {
 public:
  enum class Mode { kUsage, kDialog, kProcess, kProgressBar, kOSD, kMenu };

 private:
  QList<QStringList> sentenses_;
  Mode mode_;
  int argc_;
  char** argv_;

 public:
  Cfg(int argc, char** argv);
  ~Cfg();
  QList<QStringList> Sentenses() { return sentenses_; }
  Mode mode() { return mode_; }
  QString Usage();
  int& Argc() { return argc_; }
  char** Argv() { return argv_; }
};
