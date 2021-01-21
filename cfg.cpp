#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <QDebug>
#include <QString>
#include <QVector>
#include <algorithm>
#include <vector>
#include <string>

Cfg::Cfg(int argc, char** argv)
    : argc_(argc),
      argv_(argv),
      //mode_params_(argv_ + 2, argv_ + argc_),
      mode_(Mode::kUsage),
      setup_(nullptr) {
  for (int i=2;i<argc_;++i)mode_params_.append(argv_[i]); // для qt 5.9
  QString modes = "DPBOM";  // Возможные режимы, см. Cfg::Usage().
  if (argc_ < 2) return;  // Выводим справку!
  QChar mode = *argv_[1];
  if (!modes.contains(mode)) return;
  switch (mode.toLatin1()) {
    case 'D':
      setup_ = ModeDialog();
      mode_ = Mode::kDialog;
      break;
    case 'P':
      setup_ = ModeProcess();
      mode_ = Mode::kProcess;
      break;
    case 'B':
      mode_ = Mode::kProgressBar;
      break;
    case 'O':
      mode_ = Mode::kOSD;
      break;
    case 'M':
      mode_ = Mode::kMenu;
      break;
  }
}

Cfg::~Cfg() {}

void* Cfg::ModeDialog() {
  const QString keys = "TIRCD";
  auto char_to_enum = [](char c) {
    switch (c) {
      case 'I':
        return Cfg::ModeDialog::kInput;
      case 'R':
        return Cfg::ModeDialog::kRadio;
      case 'C':
        return Cfg::ModeDialog::kCheck;
      case 'D':
      default:
        return Cfg::ModeDialog::kDir;
    }
  };
  QString title;
  QVector<DialogEntry> setup;
  QStringList buf;
  for (auto it = mode_params_.rbegin(); it != mode_params_.rend(); ++it) {
    const QString param = *it;
    if (param.size() == 1 && keys.contains(param)) {
      if (buf.size() == 0) {
        qDebug() << "buf empty, param = " + param;
        return nullptr;
      }
      if (param == "T") {
        title = buf.front();
      } else {
        enum Cfg::ModeDialog type = char_to_enum(param.at(0).toLatin1());
        buf.removeFirst();
        setup.push_front(DialogEntry{type, buf.front(), buf});
      }
      buf.clear();
      continue;
    }
    buf.push_front(param);
  }

  if (setup.size() == 0) return nullptr;
  return new Dialog{title, setup};
}

void* Cfg::ModeProcess() {
  Process cfg;
  for (const auto& param : mode_params_) {
    if (param.contains('=') && cfg.binary.isEmpty()) {
      auto parts = param.split('=');
      auto env_name = parts.at(0);
      auto env_val = parts.at(1);
      cfg.environment.insert(qMakePair(env_name, env_val));
    } else if (cfg.binary.isEmpty()) {
      cfg.binary = param;
    } else {
      cfg.arguments.append(param);
    }
  }
  return new Process{cfg};
}
