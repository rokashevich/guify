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

#include "components/autobutton.hpp"
#include "control.hpp"
MainWindowPanel::MainWindowPanel(Cfg& cfg) : MainWindow() {
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                       Qt::X11BypassWindowManagerHint |
                       Qt::WindowStaysOnTopHint);

  QHBoxLayout* layout = new QHBoxLayout();

  // Чтобы после упаковки иконок и текстов в панель не было отступов.
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  // Настройки получаем вида:
  // QList(
  //    QList("langswitcher", "en,ru"),
  //    QList("menu", "MENUFILE"), QList("button", "/path")
  // )

  for (QStringList item : cfg.Settings()) {
    const QString type = item.takeFirst();
    if (type == "control") {
      QString path = item.takeFirst();
      Control* button = new Control(path, this);
      layout->addWidget(button);
    } else {
      const QString value = item.takeFirst();
      QLabel* label = new QLabel{type + "-" + value};
      layout->addWidget(label);
      label->setStyleSheet("*{background-color:gray;}");
    }
  }

  QWidget* window = new QWidget();
  window->setLayout(layout);
  this->setCentralWidget(window);
}
