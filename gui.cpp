#include "gui.hpp"

#include <QDebug>
#include <QEvent>

#include "cfg.hpp"
#include "mainwindowdialog.hpp"
#include "mainwindowosd.hpp"
#include "mainwindowpanel.hpp"
#include "mainwindowusage.hpp"

class ResizeEventFilter : public QObject {
  Cfg &cfg_;

 public:
  ResizeEventFilter(Cfg &cfg) : cfg_{cfg} {}
  ~ResizeEventFilter() {}

 protected:
  bool eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Resize) {
      QWidget *w = qobject_cast<QWidget *>(obj);
      cfg_.ApplyAfterShown(*w);
      return true;
    } else {
      return QObject::eventFilter(obj, event);
    }
  }
};

Gui::Gui(Cfg &cfg) : QObject() {
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

  ResizeEventFilter *filter = new ResizeEventFilter{cfg};
  mainwindow_->installEventFilter(filter);

  mainwindow_->show();

  //  QObject::connect(this, &Gui::NumberIndexChanged, mainwindow_,
  //                   &MainWindow::NumberIndexChanged);
}

void Gui::SwarmCallback(int number, int index) {
  emit NumberIndexChanged(number, index);
}

int Gui::Run() { return 1; }
