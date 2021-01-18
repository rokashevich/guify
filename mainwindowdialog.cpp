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
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

MainWindowDialog::MainWindowDialog(Cfg* cfg)
    : MainWindow(), positioning_done_(false) {
  const auto setup = static_cast<Cfg::Dialog*>(cfg->Setup());
  const auto params = static_cast<QVector<Cfg::DialogEntry>*>(&setup->params);

  setWindowTitle(setup->title);  // Заголовок пустой для чистоты UI.

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
        QPushButton* b = new QPushButton(params.at(0));
        connect(b, &QPushButton::clicked,
                [b]() { b->setText(QFileDialog::getExistingDirectory()); });
        hbl->addWidget(b);
      } break;
      default:
        break;
    }
    QGroupBox* gb = new QGroupBox;
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
      QString title = result.first;
      QStringList values = result.second;
      text +=
          title + "=" +
          (values.size() > 1 ? "(" + values.join(" ") + ") " : values.first()) +
          "\n";
    }
    fprintf(stdout, "%s", text.toStdString().c_str());
    exit(0);
  });

  QWidget* w = new QWidget;
  w->setLayout(gl);
  this->setCentralWidget(w);
}

void MainWindowDialog::NumInstancesChanged(int number, int index) {
  Q_UNUSED(number);
  Q_UNUSED(index);
  if (positioning_done_) return;
  positioning_done_ = true;
}
