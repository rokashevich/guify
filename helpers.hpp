#pragma once
#include <iostream>
#include <vector>
void o(std::string message) { std::cout << message << std::endl; }
template <typename T>
void o(const std::vector<T>& vector, std::string prefix = "",
       std::string separator = " ") {
  std::cout << prefix;
  for (auto it = vector.begin(); it != vector.end(); ++it) {
    std::cout << *it;
    if (it + 1 != vector.end()) std::cout << separator;
  }
  std::cout << std::endl;
}
void e(std::string exit_message) {
  perror(exit_message.data());
  exit(1);
}
