#include "mainwindowdialog.hpp"

#include <QChar>
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowDialog::MainWindowDialog(Cfg& cfg) : MainWindow() {
  //  const auto setup = static_cast<Cfg::Dialog*>(cfg->Variable());
  //  const auto params =
  //  static_cast<QVector<Cfg::DialogEntry>*>(&setup->params);

  setWindowTitle(cfg.Title());  // Заголовок пустой для чистоты UI.

  QGridLayout* gl = new QGridLayout;
  int row = 0;
  QList<QStringList> lines = cfg.Settings();
  for (auto& line : lines) {
    // В line настройки одной панели: QList("I", "Title", "Initial").
    const QString type = line.takeFirst();   // QList("Title", "Initial").
    const QString title = line.takeFirst();  // QList("Initial").
    QHBoxLayout* hL = new QHBoxLayout;
    if (type == "I") {
      hL->addWidget(new QLineEdit(line.size() ? line.takeLast() : ""));
    } else if (type == "R") {
      while (line.size()) hL->addWidget(new QRadioButton(line.takeFirst()));
    } else if (type == "C") {
      while (line.size()) hL->addWidget(new QCheckBox(line.takeFirst()));
    } else if (type == "D") {
      QString dir =  // Если начальный каталог никак не задан - берём $HOME.
          QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);
      if (!line.size()) {  // Начальный каталог задали из командной строки.
        dir = line.takeLast();
      } else {
        // Начальный каталог из командной строки не задан. Используем тот,
        // который был использован в прошлый вызов программы и был автосохранён
        // (с помощью QSettings).
        QSettings settings;
        const QString s = settings.value("Dialog/kDir" + title).toString();
        if (s.length()) dir = s;
      }
      QPushButton* b = new QPushButton(dir);
      // Не разрешаем кнопке быть больше, чем полэкрана.
      b->setMaximumWidth(this->AvailableWidth() / 2);
      connect(b, &QPushButton::clicked, [b, title]() {
        const QString changed_dir = QFileDialog::getExistingDirectory();
        b->setText(changed_dir);
        QSettings settings;
        settings.setValue("Dialog/kDir" + title, changed_dir);
      });
      hL->addWidget(b);
    }
    // До Qt 5.14.0 есть баг - большой отступ сверху. Нейтрализуем:
    // https://bugreports.qt.io/browse/QTBUG-44056
    QGroupBox* gb = new QGroupBox();
    gb->setStyleSheet("QGroupBox{border:0px;}");
    gb->setLayout(hL);
    gl->addWidget(new QLabel(title), row, 0);
    gl->addWidget(gb, row, 1);
    ++row;
  }
  QPushButton* b = new QPushButton("OK");
  gl->addWidget(b, row, 0, 1, 2);
  connect(b, &QPushButton::clicked, [gl]() {
    QVector<QPair<QString, QStringList>> results;
    for (int i = 0; i < gl->count() - 1; ++++i) {
      QString title = qobject_cast<QLabel*>(gl->itemAt(i)->widget())->text();
      QGroupBox* gb = qobject_cast<QGroupBox*>(gl->itemAt(i + 1)->widget());
      QHBoxLayout* hbl = qobject_cast<QHBoxLayout*>(gb->layout());
      QStringList values;
      for (int j = 0; j < hbl->count(); ++j) {
        QWidget* w = hbl->itemAt(j)->widget();
        if (w != nullptr) {
          if (QLineEdit* le = qobject_cast<QLineEdit*>(w)) {
            values.append(le->text());
          } else if (QRadioButton* rb = qobject_cast<QRadioButton*>(w)) {
            if (rb->isChecked()) values.append(rb->text());
          } else if (QCheckBox* cb = qobject_cast<QCheckBox*>(w)) {
            if (cb->isChecked()) values.append(cb->text());
          } else if (QPushButton* pb = qobject_cast<QPushButton*>(w)) {
            values.append(pb->text());
          }
        }
      }
      results.append(qMakePair(title, values));
    }

    QString text;
    for (auto& result : results) {
      QStringList values = result.second;
      if (values.count() != 0) {
        QString title = result.first;
        text += title + "=" +
                (values.size() > 1 ? "(" + values.join(" ") + ") "
                                   : values.first()) +
                "\n";
      }
    }
    fprintf(stdout, "%s", text.toStdString().c_str());
    exit(0);
  });

  QWidget* w = new QWidget;
  w->setLayout(gl);
  this->setCentralWidget(w);
}
