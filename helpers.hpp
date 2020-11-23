#pragma once
#include <sys/wait.h>
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

// Удаление элемента map по предикату.
// Появится в C++20.
// https://stackoverflow.com/questions/800955/remove-if-equivalent-for-stdmap
template <typename ContainerT, typename PredicateT>
void erase_if(ContainerT& items, const PredicateT& predicate) {
  for (auto it = items.begin(); it != items.end();) {
    if (predicate(*it))
      it = items.erase(it);
    else
      ++it;
  }
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
// вывод в виде отсортированного вектора.
inline std::vector<pid_t> PidOf(std::string program_name) {
  std::vector<pid_t> pids;
  int pfds[2];
  if (pipe(pfds) == -1) {
    std::cerr << "pipe" << std::endl;
    return {};
  }
  if (!fork()) {
    // close(1);
    dup2(pfds[1], 1);
    close(pfds[0]);
    std::string child_pid = std::to_string(getpid());
    execlp("pidof", "pidof", "-o", child_pid.data(), program_name.data(), NULL);
    std::cerr << "execlp" << std::endl;
  }
  close(pfds[1]);  // Закрывает stdin (1).
  // close(0);        // Закрываем нормальный stdin.
  dup2(pfds[0], 0);  // stdout (1) -> stdin (pfds[0])
  char arr[1000];
  read(pfds[0], arr, sizeof(arr));
  close(pfds[0]);
  char* n = strchr(arr, '\n');
  *n = 0;
  const std::string child_stdout(arr);

  // std::cout << ">>" << child_stdout << std::endl;
  //    std::cin.clear();
  //  const std::string child_stdout{std::istreambuf_iterator<char>(std::cin),
  //                                 std::istreambuf_iterator<char>()};
  std::cout << "!!!" << child_stdout << "???" << std::endl;
  std::regex e("[0-9]+");
  auto begin =
      std::sregex_iterator(child_stdout.begin(), child_stdout.end(), e);
  auto end = std::sregex_iterator();
  for (auto it = begin; it != end; ++it) {
    pids.push_back(std::stoi(it->str()));
  }
  // wait(nullptr);

  std::sort(pids.begin(), pids.end());
  return pids;
}

}  // namespace helpers
