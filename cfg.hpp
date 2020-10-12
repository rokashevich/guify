#pragma once

// Назначение класса - распарсить аргументы командной строки.
// Помимо проверки введённых параметров на правильность,
// производится поиск:
//   - самого длинного названия панели (левый столбик);
//   - самый длинный вариант;
//   - наибольшее кол-во вариантов в панели.
// Эти поля будут нужны для красивой компоновки виджетов в классе Xxdialog.

#include <iostream>
#include <vector>

class Cfg
{
 public:
  bool Init(int argc, char** argv);
  std::string Title() { return title_; }
  std::vector<std::vector<std::string> > Sentenses() { return sentenses_; }
  std::string LongestPanelName() { return longest_panel_name_; }
  std::string LongestOptionName() { return longest_option_name_; }
  int MaxOptinsCount() { return max_options_count_; }

 private:
  std::string title_{"xxdialog"};
  std::vector<std::vector<std::string> > sentenses_;
  std::string longest_panel_name_;  // Самое длинное название панели.
  std::string longest_option_name_{""};  // Самый длинный ваирант.
  int max_options_count_{0};  // Наибольшее кол-во вариантов в одной панели.
};
