#pragma once

// Задача класса - проверить синтаксическую правильность аргументов командной
// строки. Т.е. что ключи не конфликтуют и имеют нужное количество аргументов.
// Семантическая проверка делается в конкретном классе gui! Т.е. проверки что
// строковый аргумент - это директория и она существует и т.п.

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

class Cfg : public QObject {
  Q_OBJECT
 public:
  enum class Mode { kDialog, kOSD, kPanel };
  Cfg(const QStringList& arguments);
  ~Cfg();
  QVariant Variable() { return variable_; }
  QList<QStringList> Settings() { return settings_; }
  Mode mode() { return mode_; }
  QVariant DialogEntryType(const QStringList&);
  QString Title() { return title_; }
  QString ConfigError() { return config_error_; }
  void ApplyAfterShown(QWidget&);
  void Run();

 private:
  const QStringList arguments_;
  const QCommandLineOption titleOption_{"title", "Application title (optional)",
                                        "string"};
  const QCommandLineOption geometryOption_{"geometry", "Placement (optional)",
                                           "T/B/L/R"};
  const QCommandLineOption shOption_{"sh", "Interpet command with sh -c",
                                     "command"};
  void AddDialogOptions();
  void AddPanelOptions();
  void AddOsdOptions();
  QString ConfigureDialog();
  QString ConfigurePanel();
  QString ConfigureOsd();
  Process* p_;
  QCommandLineParser parser_;
  QString title_;
  Mode mode_;
  QStringList mode_params_;

  QVariant variable_;
  QList<QStringList> settings_;

  QString config_error_;
 signals:
  void processFinished(int exitCode);
};
