#include "mainwindowusage.hpp"

#include <QTextEdit>

MainWindowUsage::MainWindowUsage(Cfg* cfg) : MainWindow() {
  QTextEdit* te = new QTextEdit;
  te->setText(cfg->Usage());
  this->setCentralWidget(te);
}
