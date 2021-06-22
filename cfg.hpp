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
#include <QObject>
#include <QPair>
#include <QSet>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <QWidget>

#include "process.hpp"

// guify panel --langswitcher en,ru --menu FILE/DIR --button DIR
namespace Panel {
enum class Type { kLangSwitcher, kButton, kMenu };
struct Entry {
  Type type;
  QVariant data;
};
};  // namespace Panel

Q_DECLARE_METATYPE(Panel::Entry);

class Cfg : public QObject {
  Q_OBJECT

  const QStringList arguments_;
  const QCommandLineOption titleOption_{"title", "Application title (optional)",
                                        "string"};
  const QCommandLineOption geometryOption_{"geometry", "Placement (optional)",
                                           "T/B/L/R"};
  const QCommandLineOption shOption_{"sh", "Interpet command with sh -c",
                                     "command"};
  QString ConfigurePanel();
  void AddPanelOptions();
 signals:
  void processFinished(int exitCode);

 public:
  enum class Mode { kDialog, kOSD, kPanel };

  enum class DialogEntryType { kInput, kRadio, kCheck, kDir, kFile };
  struct DialogEntry {
    DialogEntryType type;
    QString title;
    QStringList params;
  };

 private:
  Process* p_;
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
  QVariant DialogEntryType(const QStringList&);
  QString Title() { return title_; }
  QString ConfigError() { return config_error_; }
  void ApplyAfterShown(QWidget&);
  void Run();
};
