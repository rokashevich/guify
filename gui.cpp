#include "gui.hpp"

#include <QDebug>

#include "mainwindowdialog.hpp"
#include "mainwindowprocess.hpp"
#include "mainwindowusage.hpp"

Gui::Gui(Cfg* cfg) {
  application_ = new QApplication(cfg->Argc(), cfg->Argv());
  switch (cfg->mode()) {
    case Cfg::Mode::kDialog:
      mainwindow_ = new MainWindowDialog(cfg);
      break;
    case Cfg::Mode::kProcess:
      mainwindow_ = new MainWindowProcess(cfg);
      break;
    case Cfg::Mode::kProgressBar:
      break;
    case Cfg::Mode::kOSD:
      break;
    case Cfg::Mode::kMenu:
      break;
    default:
      mainwindow_ = new MainWindowUsage;
  }
  mainwindow_->show();
}

void Gui::NumInstancesChanged(int number, int index) {
  mainwindow_->NumInstancesChanged(number, index);
}

int Gui::Run() { return application_->exec(); }
