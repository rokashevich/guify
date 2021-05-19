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

MainWindowDialog::MainWindowDialog(Cfg* cfg) : MainWindow() {
  const auto setup = static_cast<Cfg::Dialog*>(cfg->Setup());
  const auto params = static_cast<QVector<Cfg::DialogEntry>*>(&setup->params);

  setWindowTitle(cfg->Title());  // Заголовок пустой для чистоты UI.

  QGridLayout* gl = new QGridLayout;
  int row = 0;
  for (const auto& a : *params) {
    const auto type = a.type;
    const auto title = a.title;
    const auto params = a.params;
    QHBoxLayout* hbl = new QHBoxLayout;
    switch (type) {
      case Cfg::ModeDialog::kInput:
        hbl->addWidget(new QLineEdit(params.at(0)));
        break;
      case Cfg::ModeDialog::kRadio:
        for (const auto& p : params) hbl->addWidget(new QRadioButton(p));
        break;
      case Cfg::ModeDialog::kCheck:
        for (const auto& p : params) hbl->addWidget(new QCheckBox(p));
        break;
      case Cfg::ModeDialog::kDir: {
        QString initial_dir =
            QStandardPaths::standardLocations(QStandardPaths::HomeLocation)
                .at(0);
        if (params.length() != 0) {
          initial_dir = params.at(0);
        } else {
          QSettings settings;
          const QString s = settings.value("Dialog/kDir" + title).toString();
          if (s.length()) initial_dir = s;
        }
        QPushButton* b = new QPushButton(initial_dir);
        // Не разрешаем кнопке быть больше, чем полэкрана.
        b->setMaximumWidth(this->AvailableWidth() / 2);
        connect(b, &QPushButton::clicked, [b, title]() {
          const QString changed_dir = QFileDialog::getExistingDirectory();
          b->setText(changed_dir);
          QSettings settings;
          settings.setValue("Dialog/kDir" + title, changed_dir);
        });
        hbl->addWidget(b);
      } break;
      case Cfg::ModeDialog::kFile: {
        const QString path = params.length() ? params.at(0) : "";
        QPushButton* b = new QPushButton(path);
        connect(b, &QPushButton::clicked, [b, path]() {
          b->setText(QFileDialog::getOpenFileName(Q_NULLPTR, "", path));
        });
        hbl->addWidget(b);
      } break;
      default:
        break;
    }
    QGroupBox* gb = new QGroupBox();

    // До Qt 5.14.0 есть баг - большой отступ сверху. Нейтрализуем:
    // https://bugreports.qt.io/browse/QTBUG-44056
    gb->setStyleSheet("QGroupBox{border:0px;}");

    gb->setLayout(hbl);
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
