#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <QChar>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QCursor>
#include <QDebug>
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

Cfg::Cfg(const QStringList& arguments) : QObject(), config_error_("") {
  QCoreApplication::setApplicationName("guify");
  parser_.addHelpOption();
  parser_.addPositionalArgument("mode", "dialog/osd/process/progress/bar/menu");
  parser_.parse(arguments);
  const QStringList args = parser_.positionalArguments();
  const QString mode = args.isEmpty() ? QString() : args.first();

  const QCommandLineOption titleOption{"title", "Application title (optional)",
                                       "string"};
  const QCommandLineOption geometryOption{"geometry", "Placement (optional)",
                                          "T/B/L/R"};
  const QCommandLineOption shOption{"sh", "Interpet command with sh -c",
                                    "command"};
  if (mode == "dialog") {
    mode_ = Cfg::Mode::kDialog;
    parser_.clearPositionalArguments();
    parser_.addPositionalArgument("dialog", "Construct dialog", "I R B D F");
    parser_.addOption(titleOption);
    parser_.addOption(geometryOption);
    parser_.process(arguments);
    variable_ =
        ConfigureDialogVariable(parser_.positionalArguments().sliced(1));
  } else if (mode == "osd") {
    mode_ = Cfg::Mode::kOSD;
    parser_.clearPositionalArguments();
    parser_.addPositionalArgument("osd", "Onscreen message label");
    parser_.addOption({"text", "Message to display", "text"});
    parser_.addOption(geometryOption);
    parser_.addOption(shOption);
    parser_.process(arguments);
    const QStringList args = parser_.positionalArguments().sliced(1);
    variable_ = parser_.value("text");

  } else {
    parser_.showHelp();
  }

  title_ = parser_.optionNames().contains("title") && parser_.isSet("title")
               ? parser_.value("title")
               : "Guify";
  QCoreApplication::setOrganizationName(title_);

  //  parser_.addPositionalArgument("mode",
  //  "dialog/osd/process/progress/bar/menu"); parser_.addOptions({
  //      {"title", "Application title (optional)", "string"},
  //      {"dialog", "Construct an Ok/Cancel window", "params"},
  //  });
  //  parser_.setOptionsAfterPositionalArgumentsMode(
  //      QCommandLineParser::ParseAsPositionalArguments);

  //  title_ = parser_.value("title");

  //  qDebug() << parser_.optionNames();

  //  if (parser_.isSet("title"))
  //    qDebug() << "title:" << parser_.values("title").join("|");
  //  if (parser_.isSet("dialog"))
  //    qDebug() << "dialog:" << parser_.values("dialog").join("|");

  //  QMap<QString, Mode> modes2{{"Dialog", Mode::kHelp},
  //                             {"OSD", Mode::kOSD},
  //                             {"Process", Mode::kProcess},
  //                             {"Progress", Mode::kProgress},
  //                             {"Menu", Mode::kMenu}};
  //  QStringList args = parser_.positionalArguments();
  //  if (QString mode = args.length() == 1 ? args.at(0) : "";
  //      modes2.contains(mode)) {
  //    mode_ = modes2.find(mode).value();
  //  } else {
  //    mode_ = Mode::kHelp;
  //  }

  //  qDebug() << "1rest:" << parser_.positionalArguments().join("|");

  //  // source is args.at(0), destination is args.at(1)

  //  for (int i = 0; i < argc_; ++i) mode_params_.append(argv_[i]);

  //  mode_params_.pop_front();  // Удаляем имя самой программы.

  //  // Заголовок задали?
  //  if (mode_params_.length() > 1 && mode_params_.at(0).length() > 1) {
  //    title_ = mode_params_.at(0);
  //    mode_params_.pop_front();
  //  } else {
  //    title_ = "guify";
  //  }

  //  if (mode_params_.length() == 0) {
  //    config_error_ = "No arguments provided!";
  //    return;
  //  }

  //  QChar mode = mode_params_.at(0).at(0);
  //  mode_params_.pop_front();

  //  QString modes = "DPBOM";
  //  if (!modes.contains(mode)) {
  //    config_error_ = "Unknown mode (" + QString(mode) + ")!";
  //    return;
  //  }

  //  switch (mode.toLatin1()) {
  //    case 'D':
  //      setup_ = ModeDialog();
  //      mode_ = Mode::kDialog;
  //      break;
  //    case 'P':
  //      setup_ = ModeProcess();
  //      mode_ = Mode::kProcess;
  //      break;
  //    case 'B':
  //      mode_ = Mode::kProgress;
  //      break;
  //    case 'O':
  //      mode_ = Mode::kOSD;
  //      break;
  //    case 'M':
  //      mode_ = Mode::kMenu;
  //      break;
  //  }
}

Cfg::~Cfg() {}

void Cfg::Run() {
  if (parser_.optionNames().contains("sh") && parser_.isSet("sh")) {
    p_ = new Process(parser_.value("sh"));
    connect(p_, &Process::finished,
            [this](int exitCode) { emit processFinished(exitCode); });
  }
}

QVariant Cfg::ConfigureDialogVariable(const QStringList& args) {
  const QString keys = "IRCDF";
  auto char_to_enum = [this](char c) {
    switch (c) {
      case 'I':
        return Cfg::ConfigureDialogVariable::kInput;
      case 'R':
        return Cfg::ConfigureDialogVariable::kRadio;
      case 'C':
        return Cfg::ConfigureDialogVariable::kCheck;
      case 'D':
        return Cfg::ConfigureDialogVariable::kDir;
      case 'F':
        return Cfg::ConfigureDialogVariable::kFile;
      default:
        this->config_error_ = "Unknown dialog mode (" + QString(c) + ")!";
        return static_cast<enum ConfigureDialogVariable>(c);
    }
  };
  QVector<DialogEntry> setup;
  QStringList buf;
  for (auto it = args.rbegin(); it != args.rend(); ++it) {
    const QString param = *it;
    if (param.size() == 1 && keys.contains(param)) {
      if (buf.size() == 0) {
        qDebug() << "buf empty, param = " + param;
        return QVariant();
      }
      enum Cfg::ConfigureDialogVariable type =
          char_to_enum(param.at(0).toLatin1());
      const QString title = buf.takeFirst();
      const QStringList params = buf;
      setup.push_front(DialogEntry{type, title, params});
      buf.clear();
      continue;
    }
    buf.push_front(param);
  }
  if (setup.size() == 0) return QVariant();
  return QVariant::fromValue(setup);
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
