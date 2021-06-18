#include "gui.hpp"

#include <QDebug>

#include "mainwindowdialog.hpp"
#include "mainwindowosd.hpp"
#include "mainwindowpanel.hpp"
#include "mainwindowusage.hpp"

Gui::Gui(Cfg& cfg) : QObject() {
  if (cfg.ConfigError().length() > 0)
    mainwindow_ = new MainWindowUsage(cfg);
  else {
    switch (cfg.mode()) {
      case Cfg::Mode::kDialog:
        mainwindow_ = new MainWindowDialog(cfg);
        break;
      case Cfg::Mode::kOSD:
        mainwindow_ = new MainWindowOsd(cfg);
        break;
      case Cfg::Mode::kPanel:
        mainwindow_ = new MainWindowPanel(cfg);
        break;
      default:
        break;
    }
  }

  mainwindow_->show();
  cfg.ApplyAfterShown(*mainwindow_);
  //  QObject::connect(this, &Gui::NumberIndexChanged, mainwindow_,
  //                   &MainWindow::NumberIndexChanged);
}

void Gui::SwarmCallback(int number, int index) {
  emit NumberIndexChanged(number, index);
}

int Gui::Run() { return 1; }
