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
  // Настройки получаем вида:
  // QList(
  //    QList("langswitcher", "en,ru"),
  //    QList("menu", "MENUFILE"), QList("button", "/path")
  // )
  qDebug() << cfg.Settings();

  for (QStringList item : cfg.Settings()) {
    const QString type = item.takeFirst();
    if (type == "button") {
      QString path = item.takeFirst();
      Button* button = new Button(path);
      layout->addWidget(button);
    } else {
      const QString value = item.takeFirst();
      QLabel* label = new QLabel{type + "-" + value};
      layout->addWidget(label);
    }
  }

  QWidget* window = new QWidget();
  window->setLayout(layout);
  this->setCentralWidget(window);
}
