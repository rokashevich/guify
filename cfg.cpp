#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <algorithm>
bool Cfg::Init(int argc, char** argv) {
  type_ = Type::kDialog;
  std::vector<std::string> possible_args = {"-T", "-I", "-C", "-R", "-D", "-P"};
  for (int i = 1; i < argc; ++i) {
    QString arg = QString(argv[i]);
    if (arg == "-T") {
      if (argc > i + 1 &&
          !std::any_of(possible_args.begin(), possible_args.end(),
                       [=](std::string k) { return k == argv[i + 1]; })) {
        title_ = argv[++i];
        continue;
      } else {
        return false;
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

  return sentenses_.size();
}
