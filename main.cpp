#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <algorithm>
#include <csignal>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "cfg.hpp"
#include "gui.hpp"
#include "helpers.hpp"
#include "swarm.hpp"
#include "unused.hpp"
int usage() {
  // Синтаксис справки по аргументам командной строки на основе:
  // https://developers.google.com/style/code-syntax
  std::cerr << R"(Usage:
  fltkdialog [-T] { -P | [-I] [-C] [-R] [-D] [-F] }
Key description:
  -T Window Title; Without parameter window is borderless
  -I var_name [initial string];
  -C var_name option1 ...;
  -R var_name option1 option2 ...; 
  -D var_name [initial directory]; default=~
  -F var_name [initial file]; default=empty
  -P process_binary [arg1 ...])"
            << std::endl;
  return 1;
}

int main(int argc, char** argv) {
  Cfg* cfg = new Cfg();
  if (!cfg->Init(argc, argv)) {
    exit(usage());
  }
  Gui* gui = new Gui(argc, argv, cfg);
  Swarm& swarm = Swarm::Singleton();
  swarm.Start(std::bind(&Gui::NumInstancesChanged, gui, std::placeholders::_1,
                        std::placeholders::_2));
  return gui->Run();
}
