#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <QDebug>
#include <QString>
#include <QVector>
#include <algorithm>
#include <string>
#include <vector>

Cfg::Cfg(int argc, char** argv)
    : argc_(argc), argv_(argv), setup_(nullptr), config_error_("") {
  for (int i = 0; i < argc_; ++i) mode_params_.append(argv_[i]);

  mode_params_.pop_front();  // Удаляем имя самой программы.

  // Заголовок задали?
  if (mode_params_.length() > 1 && mode_params_.at(0).length() > 1) {
    title_ = mode_params_.at(0);
    mode_params_.pop_front();
  }

  if (mode_params_.length() == 0) {
    config_error_ = "No arguments provided!";
    return;
  }

  QChar mode = mode_params_.at(0).at(0);
  mode_params_.pop_front();

  QString modes = "DPBOM";
  if (!modes.contains(mode)) {
    config_error_ = "Unknown mode (" + QString(mode) + ")!";
    return;
  }

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
  const QString keys = "IRCDF";
  auto char_to_enum = [this](char c) {
    switch (c) {
      case 'I':
        return Cfg::ModeDialog::kInput;
      case 'R':
        return Cfg::ModeDialog::kRadio;
      case 'C':
        return Cfg::ModeDialog::kCheck;
      case 'D':
        return Cfg::ModeDialog::kDir;
      case 'F':
        return Cfg::ModeDialog::kFile;
      default:
        this->config_error_ = "Unknown dialog mode (" + QString(c) + ")!";
        return static_cast<enum ModeDialog>(c);
    }
  };
  QVector<DialogEntry> setup;
  QStringList buf;
  for (auto it = mode_params_.rbegin(); it != mode_params_.rend(); ++it) {
    const QString param = *it;
    if (param.size() == 1 && keys.contains(param)) {
      if (buf.size() == 0) {
        qDebug() << "buf empty, param = " + param;
        return nullptr;
      }
      enum Cfg::ModeDialog type = char_to_enum(param.at(0).toLatin1());
      const QString title = buf.takeFirst();
      const QStringList params = buf;
      setup.push_front(DialogEntry{type, title, params});
      buf.clear();
      continue;
    }
    buf.push_front(param);
  }
  if (setup.size() == 0) return nullptr;
  return new Dialog{setup};
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
