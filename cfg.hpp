#pragma once

// Назначение класса - распарсить аргументы командной строки.
// Помимо проверки введённых параметров на правильность,
// производится поиск:
//   - самого длинного названия панели (левый столбик);
//   - самый длинный вариант;
//   - наибольшее кол-во вариантов в панели.
// Эти поля будут нужны для красивой компоновки виджетов в классе Xxdialog.

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QList>
#include <QPair>
#include <QSet>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <QWidget>
class Cfg {
 public:
  enum class Mode { kDialog, kOSD, kProcess, kProgress, kMenu, kHelp };

  enum class ConfigureDialogVariable { kInput, kRadio, kCheck, kDir, kFile };
  struct DialogEntry {
    ConfigureDialogVariable type;
    QString title;
    QStringList params;
  };
  struct Process {
    QSet<QPair<QString, QString>> environment;
    QString binary;
    QStringList arguments;
  };

 private:
  QCommandLineParser parser_;
  QString title_;
  Mode mode_;
  QStringList mode_params_;

  QVariant variable_;

  QString config_error_;

 public:
  Cfg(const QStringList& arguments);
  ~Cfg();
  QVariant Variable() { return variable_; }
  Mode mode() { return mode_; }
  QVariant ConfigureDialogVariable(const QStringList&);
  void* ModeProcess();
  QString Title() { return title_; }
  QString ConfigError() { return config_error_; }
  void ApplyAfterShown(QWidget&);
};
