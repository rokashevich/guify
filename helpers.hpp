#pragma once
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
namespace helpers {

inline void MessageDebug(std::string message) {
  std::cout << message << std::endl;
}

template <typename T>
void MessageDebug(const std::vector<T>& vector, std::string prefix = "",
                  std::string separator = " ") {
  std::cout << prefix;
  for (auto it = vector.begin(); it != vector.end(); ++it) {
    std::cout << *it;
    if (it + 1 != vector.end()) std::cout << separator;
  }
  std::cout << std::endl;
}

inline void MessageError(std::string exit_message) {
  perror(exit_message.data());
  exit(1);
}

// Возвращает название программы по её PIDу.
// Используем /proc/PID/cmdline.
inline std::string GetProgramNameByPid(pid_t pid) {
  std::ifstream file("/proc/" + std::to_string(pid) + "/cmdline");
  if (!file.good()) return "";
  std::string path;
  char c;
  while (file.get(c)) {
    if (c != 0)
      path.push_back(c);
    else
      break;
  }
  const int slash_pos = path.find_last_of('/');
  if (slash_pos) return path.substr(slash_pos + 1, path.size());
  return path;
}

// Вызывает системную утилиту pidof с аргументом program_name и возвращает её
// вывод.
inline std::vector<pid_t> PidOf(std::string program_name) {
  std::vector<pid_t> pids;
  int pfds[2];
  pipe(pfds);
  if (!fork()) {
    close(1);
    dup(pfds[1]);
    close(pfds[0]);
    execlp("pidof", "pidof", program_name.data(), NULL);
  } else {
    close(0);
    dup(pfds[0]);  // Получаем неотсортированную строку pid'ов вида:
    close(pfds[1]);  // "1233 3242 5454 5677"
    const std::string child_stdout{std::istreambuf_iterator<char>(std::cin),
                                   std::istreambuf_iterator<char>()};
    std::regex e("[0-9]+");
    auto begin =
        std::sregex_iterator(child_stdout.begin(), child_stdout.end(), e);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
      pids.push_back(std::stoi(it->str()));
    }
  }
  return pids;
}

}  // namespace helpers
