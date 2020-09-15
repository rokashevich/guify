#include "mainwindow.h"

#include <iostream>

#include <QApplication>

#include "cfg.h"

int usage() {
  // Принцип использования скобок:
  //   [для опциональных аргументов]
  //   <для обязательный оругментов>
  //   {для стандартных значений}
  //   (прочее)

  std::cerr << "Usage:" << std::endl;
  std::cerr << "    -T <title>" << std::endl;
  std::cerr << "    -I <input name> [default value]" << std::endl;
  std::cerr << "    -C <checkboxes name> <box 1 name> [other boxes]"
            << std::endl;
  std::cerr << "    -R <radio buttons name> <button 1 name> <button 2 name> "
               "[other buttons]"
            << std::endl;
  std::cerr << "    -D <directory selector name> {initial directory|~}"
            << std::endl;
  std::cerr << "    -P <shell command to see running>" << std::endl;
  return 1;
}

int main(int argc, char* argv[]) {
  Cfg cfg;
  if (!cfg.Init(argc, argv)) {
    exit(usage());
  }

  QApplication a(argc, argv);
  MainWindow w(cfg);
  w.show();
  return a.exec();
}
