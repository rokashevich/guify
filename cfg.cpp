#include "cfg.hpp"

#include <pwd.h>
#include <unistd.h>

#include <algorithm>
bool Cfg::Init(int argc, char** argv) {
  std::vector<std::string> possible_args = {"-T", "-I", "-C", "-R", "-D"};
  for (int i = 1; i < argc; ++i) {
    std::string arg = std::string(argv[i]);
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
                           [=](std::string i) { return i == arg; })) {
      std::vector<std::string> sentense = {arg};
      for (++i; i < argc; ++i) {
        arg = std::string(argv[i]);
        if (std::any_of(possible_args.begin(), possible_args.end(),
                        [=](std::string i) { return i == arg; })) {
          --i;
          break;
        }
        sentense.push_back(arg);
      }
      sentenses_.push_back(sentense);
    }
  }

  // Выставляем значения по умолчанию если не заданы.
  for (std::vector<std::string>& sentense : sentenses_) {
    if (sentense.at(0) == "-D") {
      if (sentense.size() == 2) {
        struct passwd* pw = getpwuid(getuid());
        const char* initial_directory = pw->pw_dir;
        sentense.push_back(std::string(initial_directory));
      }
    }
    if (sentense.at(0) == "-I") {
      if (sentense.size() == 2) {
        sentense.push_back("");
      }
    }
  }

  for (const std::vector<std::string>& sentense : sentenses_) {
    // Ищем самое длинное название панели.
    std::string label = sentense.at(1);
    if (label.length() > longest_panel_name_.length())
      longest_panel_name_ = label;

    // Ищем самый длинный вариант и набольшее кол-во вариантов.
    int options_in_this_sentense = 0;
    std::vector<std::string> options = sentense;
    options.erase(options.begin(), options.begin() + 2);
    for (const std::string& option : options) {
      ++options_in_this_sentense;
      if (option.length() > longest_option_name_.length()) {
        longest_option_name_ = option;
      }
    }
    if (options_in_this_sentense > max_options_count_)
      max_options_count_ = options_in_this_sentense;
  }

  return sentenses_.size() || longest_panel_name_.size() ||
         longest_option_name_.size() || max_options_count_;
}
