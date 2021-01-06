#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <QString>
#include <algorithm>

Cfg::Cfg(int argc, char** argv) : argc_(argc), argv_(argv) {
  const QString modes = "DPBOM";  // Возможные режимы, см. Cfg::Usage().
  mode_ = Mode::kUsage;  // Режим по умолчанию - выводить справку.
  if (argc_ < 2) return;  // Выводим справку!
  QChar mode = *argv_[1];
  if (!modes.contains(mode)) return;
  switch (mode.toLatin1()) {
    case 'D':
      break;
    case 'P':
      break;
    case 'B':
      break;
    case 'O':
      break;
    case 'M':
      break;
    default:
      return;  // Выводим справку!
  }
  std::vector<std::string> possible_args = {"-T", "-I", "-C", "-R", "-D", "-P"};
  for (int i = 1; i < argc; ++i) {
    QString arg = QString(argv[i]);
    if (arg == "-T") {
      if (argc > i + 1 &&
          !std::any_of(possible_args.begin(), possible_args.end(),
                       [=](std::string k) { return k == argv[i + 1]; })) {
        continue;
      }
    } else if (std::any_of(possible_args.begin(), possible_args.end(),
                           [=](std::string s) {
                             return QString::fromStdString(s) == arg;
                           })) {
      QStringList sentense = {arg};
      for (++i; i < argc; ++i) {
        arg = QString(argv[i]);
        if (std::any_of(possible_args.begin(), possible_args.end(),
                        [=](std::string s) {
                          return QString::fromStdString(s) == arg;
                        })) {
          --i;
          break;
        }
        sentense.push_back(arg);
      }
      sentenses_.push_back(sentense);
    }
  }

  // Выставляем значения по умолчанию если не заданы.
  for (auto& sentense : sentenses_) {
    if (sentense.at(0) == "-D") {
      if (sentense.size() == 2) {
        struct passwd* pw = getpwuid(getuid());
        QString initial_directory = pw->pw_dir;
        sentense.push_back(initial_directory);
      }
    }
    if (sentense.at(0) == "-I") {
      if (sentense.size() == 2) {
        sentense.push_back("");
      }
    }
  }
}

Cfg::~Cfg() {}

QString Cfg::Usage() {
  return R"(bguify { D | P | B | O | M }
    D (for Dialog) options:
      I text_input_var_name [initial string];
      C checkbox_var_name option1 ...;
      R radio_var_name option1 option2 ...;
      D directory_var_name [initial directory]; default=~

    P (for Process) options:
      process_binary [arg1 ...]

    B (for progressbar)
    O (for OSD)

    M (for piped menu) options:
      openbox syntaxed pipe menu script | folder)";
}
