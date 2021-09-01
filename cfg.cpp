#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <QChar>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QMap>
#include <QObject>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QString>
#include <QVector>
#include <algorithm>
#include <process.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "process.hpp"

Cfg::Cfg(const QStringList& arguments)
    : QObject(), arguments_{arguments}, config_error_("") {
  QCoreApplication::setApplicationName("guify");
  QCoreApplication::setApplicationVersion(VERSION);
  parser_.addHelpOption();
  parser_.addVersionOption();
  parser_.process(arguments);

  parser_.addPositionalArgument("mode", "dialog/osd/panel");
  parser_.parse(arguments);

  const QStringList args = parser_.positionalArguments();
  const QString mode = args.isEmpty() ? QString() : args.first();
  QString errorMessage = "";
  if (mode == "dialog") {
    mode_ = Cfg::Mode::kDialog;
    errorMessage = ConfigureDialog();
  } else if (mode == "osd") {
    mode_ = Cfg::Mode::kOSD;
    errorMessage = ConfigureOsd();
  } else if (mode == "panel") {
    errorMessage = ConfigurePanel();
  } else {
    errorMessage = "Bad mode `" + mode + "`";
  }

  if (!errorMessage.isEmpty()) {
    // TODO В случае обнаружения ошибок конфигурирования прокидывать их в gui
    // для показа.
    qWarning() << errorMessage;
    parser_.showHelp();
  }

  title_ = parser_.optionNames().contains("title") && parser_.isSet("title")
               ? parser_.value("title")
               : "Guify";
  QCoreApplication::setOrganizationName(title_);

  if (parser_.optionNames().contains("sh") && parser_.isSet("sh")) {
    p_ = new Process(parser_.value("sh"));
    connect(p_, &Process::finished,
            [this](int exitCode) { emit processFinished(exitCode); });
  }
}

Cfg::~Cfg() {}
void Cfg::AddDialogOptions() {
  parser_.clearPositionalArguments();
  parser_.addPositionalArgument("dialog", "Construct dialog", "I R B D F");
  parser_.addOption(titleOption_);
  parser_.addOption(geometryOption_);
  parser_.process(arguments_);
}
void Cfg::AddPanelOptions() {
  parser_.clearPositionalArguments();
  parser_.addOption(
      {"langswitcher", "Generate panels based on a preapared folder", "path"});
  parser_.addOption({"control",
                     "Generate control button based on a preapared folder",
                     "path"});
  parser_.addOption(
      {"menu", "Generate panels based on a preapared folder", "path"});
  parser_.addOption(geometryOption_);
  parser_.process(arguments_);
}
void Cfg::AddOsdOptions() {
  parser_.clearPositionalArguments();
  parser_.addPositionalArgument("osd", "Onscreen message label");
  parser_.addOption({"text", "Message to display", "text"});
  parser_.addOption(geometryOption_);
  parser_.addOption(shOption_);
  parser_.process(arguments_);
}
QString Cfg::ConfigureDialog() {
  AddDialogOptions();
  QStringList entry;
  const int offset = 2;
  QStringList dialogArguments = arguments_.sliced(offset);
  std::reverse(dialogArguments.begin(), dialogArguments.end());
  for (auto& argument : dialogArguments) {
    entry.push_front(argument);
    const QString keys = "IRCDF";
    if (argument.size() == 1 && keys.contains(QChar(argument.at(0)))) {
      if (entry.size() == 0) return "Argement `" + argument + "` is empty.";
      settings_.push_front(entry);
      entry.clear();
    }
  }
  return "";
}
QString Cfg::ConfigurePanel() {
  mode_ = Cfg::Mode::kPanel;
  AddPanelOptions();
  QList<QVariant> variable;
  QMap<QString, int> counter;
  for (auto& optionName : parser_.optionNames()) {
    const QStringList possible{"menu", "langswitcher", "control"};
    if (!possible.contains(optionName)) continue;
    const int index = counter.contains(optionName) ? counter[optionName] : 0;
    counter.insert(optionName, index + 1);

    QString value = parser_.values(optionName).at(index);
    settings_.append(QStringList{optionName, value});
  }
  variable_ = variable;
  return "";
}
QString Cfg::ConfigureOsd() {
  AddOsdOptions();
  settings_.append({parser_.value("text")});
  return "";
}
void Cfg::Run() {
  if (parser_.optionNames().contains("sh") && parser_.isSet("sh")) {
    p_ = new Process(parser_.value("sh"));
    connect(p_, &Process::finished,
            [this](int exitCode) { emit processFinished(exitCode); });
  }
}

void Cfg::ApplyAfterShown(QWidget& w) {
  const QString geometry =
      parser_.optionNames().contains("geometry") && parser_.isSet("geometry")
          ? parser_.value("geometry")
          : "";

  QScreen* screen = qApp->screenAt(QCursor::pos());
  if (!screen) screen = w.screen();
  const QRect screenRect = screen->availableGeometry();
  const QSize widget(w.size());
  const QSize availableSize(screenRect.width() - widget.width(),
                            screenRect.height() - widget.height());
  const QPoint middle(screenRect.x() + availableSize.width() / 2,
                      screenRect.y() + availableSize.height() / 2);
  const int half = 50;
  QPoint shift(0, 0);
  for (QChar c : geometry) {
    if (c == 'T') {
      shift.setY(shift.y() - half);
    } else if (c == 'B') {
      shift.setY(shift.y() + half);
    } else if (c == 'L') {
      shift.setX(shift.x() - half);
    } else if (c == 'R') {
      shift.setX(shift.x() + half);
    }
  }
  QPoint move(middle.x() + availableSize.width() * shift.x() / 100,
              middle.y() + availableSize.height() * shift.y() / 100);
  w.move(move.x(), move.y());
}
