#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <QDebug>
#include <QString>
#include <algorithm>

Cfg::Cfg(int argc, char** argv)
    : argc_(argc),
      argv_(argv),
      mode_params_(argv_ + 2, argv_ + argc_),
      mode_(Mode::kUsage),
      setup_(nullptr) {
  QString modes = "DPBOM";  // Возможные режимы, см. Cfg::Usage().
  if (argc_ < 2) return;  // Выводим справку!
  QChar mode = *argv_[1];
  if (!modes.contains(mode)) return;
  switch (mode.toLatin1()) {
    case 'D':
      setup_ = SetupDialog();
      mode_ = Mode::kDialog;
      break;
    case 'P':
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

void* Cfg::SetupDialog() {
  const QString keys = "IRCD";
  auto char_to_enum = [](char c) {
    switch (c) {
      case 'I':
        return Cfg::SetupDialog::kInput;
      case 'R':
        return Cfg::SetupDialog::kRadio;
      case 'C':
        return Cfg::SetupDialog::kCheck;
      case 'D':
      default:
        return Cfg::SetupDialog::kDir;
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
      enum Cfg::SetupDialog type = char_to_enum(param.at(0).toLatin1());
      const QString title = buf.front();
      buf.removeFirst();
      setup.push_front(DialogEntry{type, title, buf});
      buf.clear();
      continue;
    }
    buf.push_front(param);
  }

  if (setup.size() == 0) return nullptr;
  return new QVector<DialogEntry>{setup};
}
