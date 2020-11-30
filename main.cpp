#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
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

int Reconfigure() { return 1; }

int main(int argc, char** argv) {
  Cfg* cfg = new Cfg();
  if (!cfg->Init(argc, argv)) {
    exit(usage());
  }
  Fl::lock();  // https://www.fltk.org/doc-1.3/advanced.html
  Gui* gui = new Gui(cfg);
  Swarm swarm = Swarm::Singleton();
  swarm.Start(std::bind(&Gui::NumInstancesChanged, gui, std::placeholders::_1,
                        std::placeholders::_2));
  return gui->Run();
}
