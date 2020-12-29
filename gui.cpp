#include "gui.hpp"

#include "mainwindowdialog.hpp"

Gui::Gui(int argc, char* argv[], Cfg* cfg) {
  application_ = new QApplication(argc, argv);
  if (cfg->type() == Cfg::Type::kDialog)
    mainwindow_ = new MainWindowDialog(cfg);
  mainwindow_->show();
}

void Gui::NumInstancesChanged(int number, int index) {
  mainwindow_->NumInstancesChanged(number, index);
}

int Gui::Run() { return application_->exec(); }
