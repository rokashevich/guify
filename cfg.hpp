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
  enum class Type { kDialog, kProcess, kProgress, kOSD };

 private:
  std::string title_{"xxdialog"};
  QList<QStringList> sentenses_;
  Type type_;

 public:
  bool Init(int argc, char** argv);
  std::string Title() { return title_; }
  QList<QStringList> Sentenses() { return sentenses_; }
  Type type() { return type_; }
};
