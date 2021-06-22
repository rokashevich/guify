#include "mainwindowpanel.hpp"

#include <QChar>
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include "button.hpp"

MainWindowPanel::MainWindowPanel(Cfg& cfg) : MainWindow() {
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                       Qt::X11BypassWindowManagerHint |
                       Qt::WindowStaysOnTopHint);
  QHBoxLayout* layout = new QHBoxLayout();
  //  QString text;
  for (auto& variant : cfg.Variable().toList()) {
    const Panel::Entry entry = variant.value<Panel::Entry>();
    if (entry.type == Panel::Type::kButton) {
      Button* button = new Button(entry.data.toString());
      layout->addWidget(button);
    } else {
      QLabel* label = new QLabel{entry.data.toString()};
      layout->addWidget(label);
    }
  }

  QWidget* window = new QWidget();
  window->setLayout(layout);
  this->setCentralWidget(window);
}
