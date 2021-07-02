#pragma once
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QStringList>
#include <QTimerEvent>
#include <QToolButton>
#include <QWidget>

#include "icon.hpp"

class AutoButton : public QToolButton {
  Q_OBJECT

  const QString sandbox_;
  const QString icon_path_;
  Icon *icon_;

  typedef void (AutoButton::*pmemfunc_t)();
  QPair<QStringList, pmemfunc_t> scripts_;
  const QList<QPair<QStringList, pmemfunc_t>> possible_scripts_{
      {{"detach.sh"}, &AutoButton::ClickDetach},
      {{"start.sh", "stop.sh"}, &AutoButton::ClickStartStop}};

  void ClickDetach() {
    const auto &detach_script_name{scripts_.first.at(0)};
    const auto &detach_script_path{QDir(sandbox_).filePath(detach_script_name)};
    //    qDebug() << detach_script_path;
    _d->is_start = false;
    _d->is_stop = false;
    _d->is_detach = true;
    _d->dir = "/";
    _d->disableTimeout = 0;
    QFileInfo fi(detach_script_path);
    _d->detachedArgs = QStringList{};
    _d->detachedFn = fi.absoluteFilePath();
  }
  void ClickStartStop() {
    const auto &start_script_name{scripts_.first.at(0)};
    const auto &stop_script_name{scripts_.first.at(1)};
    const auto &start_script_path{QDir(sandbox_).filePath(start_script_name)};
    const auto &stop_script_path{QDir(sandbox_).filePath(stop_script_name)};

    _d->is_detach = false;

    _d->startArgs = QStringList{};
    _d->is_start = true;
    _d->startFn = start_script_path;
    _d->m_process.setProgram(_d->m_shell);
    QStringList sl;
    sl << _d->preArgs;
    sl << _d->startFn;
    sl << _d->startArgs;
    _d->m_process.setArguments(sl);

    _d->detachedArgs = QStringList{};
    _d->is_stop = true;
    _d->detachedFn = stop_script_path;
  }

 public:
  AutoButton(const QString &sandbox, QWidget *parent = nullptr)
      : QToolButton(parent),
        sandbox_(sandbox),
        icon_path_(QDir(sandbox_).filePath("icon.svg")),
        _d(new Private()) {
    const QString verdict{SandboxSetup()};
    if (!verdict.isEmpty()) {
      setText(verdict);
      return;
    }

    icon_ = new Icon(icon_path_);
    setIcon(icon_->Qicon());
    setIconSize(icon_->minimumSize());
    init();

    // Регистрируем скрипты обрабаотки нажатий.
    const pmemfunc_t f = scripts_.second;
    (this->*f)();
  }

  QString SandboxSetup() {
    if (!QFile(icon_path_).exists()) {
      return "No `" + icon_path_ + "`";
    }

    QList<QPair<QStringList, pmemfunc_t>> variants_found;
    for (const auto &variant : possible_scripts_) {
      const auto num_scripts_in_variant{variant.first.size()};
      auto num_scripts_found{0};
      for (const auto &script_name : variant.first) {
        const auto script_path{QDir(sandbox_).filePath(script_name)};
        if (!QFile(script_path).exists()) {
          break;
        }
        ++num_scripts_found;
      }
      if (num_scripts_found == num_scripts_in_variant) {
        variants_found.append(variant);
      } else if (num_scripts_found > 0) {
        return "Inconsistent scripts";
      }
    }
    scripts_ = variants_found.first();
    return "";
  }

  ~AutoButton() {
    if (_d->m_process.state() == QProcess::Running) {
      _d->m_process.terminate();
      _d->m_process.kill();
    }
    delete _d;
  }

  // задать всплывающее описание - то же самое, что setToolTip
  // void setDescription(const QString &description);

  // задать подтверждение, действия по умолчанию описываются флагами
  void setConfirm(const QString &text, const QString &info, int flags = 0) {
    Q_UNUSED(flags);
    _d->confirmText = text;
    _d->infoText = info;
    _d->is_confirm = (!_d->confirmText.isEmpty()) || (!_d->infoText.isEmpty());
  }

  // таймаут завершения процесса - для консольных приложений достаточно 10 мс
  void setKillTimeout(int timeout) { _d->killTimeout = timeout; }

  // задать стиль кнопки в обычном состоянии
  void setStyleSheetNormal(QString s) { _d->styleSheetNormal = s; }

  // задать стиль кнопки в состоянии выполнения процесса (между start и stop)
  void setStyleSheetPressed(QString s) { _d->styleSheetPressed = s; }

 signals:

  void StdError(const QByteArray &msg);

  void StdOutput(const QByteArray &msg);

  // код завершения
  void ExitCode(int);

  // прогресс 0..100
  void Progress(int);

 protected:
  static const int NoTimerId = -1;
  const char *const scanProgress = "progress=int ";

  virtual void timerEvent(QTimerEvent *event) override {
    if (event->timerId() == _d->disableTimerId) {
      if (!isEnabled()) {
        killTimer(_d->disableTimerId);
        _d->disableTimerId = NoTimerId;
        setEnabled(true);
      }
    }
    if (event->timerId() == _d->checkTimerId) {
      // проблема - не запущено
      killTimer(_d->checkTimerId);
      _d->checkTimerId = NoTimerId;
    }
  }

 private slots:

  virtual void actionButtonClicked() {
    if (_d->is_confirm &&
        (_d->m_process.state() == QProcess::NotRunning)) {  // первый клик

      QMessageBox msgBox;
      if (!_d->confirmText.isEmpty()) msgBox.setText(_d->confirmText);
      if (!_d->infoText.isEmpty()) msgBox.setInformativeText(_d->infoText);
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      int rc = msgBox.exec();
      switch (rc) {
        case QMessageBox::Yes:
          break;
        default:
          rc = QMessageBox::NoButton;
      }
      if (rc == QMessageBox::NoButton) return;
    }
    actionButtonDone();
  }

  virtual void actionButtonDone() {
    if (_d->is_detach) {
      if (_d->disableTimerId == NoTimerId) {
        _d->disableTimerId = startTimer(_d->disableTimeout);
      }
      // emit StdOutput(QByteArray("detached..."));
      setDisabled(true);
      QStringList sl;
      sl << _d->preArgs;
      sl << _d->detachedFn;
      sl << _d->detachedArgs;
      if (_d->dir.isEmpty())
        QProcess::startDetached(_d->m_shell, sl);
      else
        QProcess::startDetached(_d->m_shell, sl, _d->dir);
    } else {
      if (_d->m_process.state() == QProcess::NotRunning) {  // первый клик
        if (_d->is_start) {
          if (_d->checkTimerId == NoTimerId) {
            _d->checkTimerId = startTimer(5000);
          }
          if (_d->disableTimerId == NoTimerId) {
            setDisabled(true);
            _d->disableTimerId = startTimer(_d->disableTimeout);
          }
          // emit StdOutput("starting...
          // "+_d->m_process.program().toLocal8Bit());
          _d->m_process.start();
          this->setStyleSheet(_d->styleSheetPressed);
        }
      } else {  // Running...
        if (_d->is_stop) {
          // emit StdOutput("stop...");
          QStringList sl;
          sl << _d->preArgs;
          sl << _d->detachedFn;
          sl << _d->detachedArgs;
          emit StdOutput(_d->detachedFn.toUtf8());
          QProcess::startDetached(_d->m_shell, sl);
        } else {
          QProcess::startDetached(
              QString("pkill -P %1").arg(_d->m_process.processId()));
        }
      }
    }
  }

  void processStdout() {
    while (_d->m_process.canReadLine()) {
      QByteArray bA = _d->m_process.readLine();
      if (bA.startsWith(scanProgress)) {
        int n = std::stoi(bA.data() + strlen(scanProgress));
        emit Progress(n);
      } else {
        emit StdOutput(bA);
      }
    }
  }

  void processStderr() {
    while (_d->m_process.canReadLine()) {
      QByteArray bA = _d->m_process.readLine();
      emit StdError(bA);
    }
  }

  void processError(QProcess::ProcessError error) {
    emit StdError((QString("error %1").arg(error)).toUtf8());
  }

  void processStarted() {
    if (NoTimerId != _d->checkTimerId) {
      killTimer(_d->checkTimerId);
      _d->checkTimerId = NoTimerId;
    }
  }

  void processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
      emit StdError(QString("crashed with code %1").arg(exitCode).toUtf8());
    }
    emit ExitCode(exitCode);
    if (this->isCheckable() && this->isChecked()) {
      this->setChecked(false);
    }
    this->setStyleSheet(_d->styleSheetNormal);
  }

 private:
  void init() {
    if (text().isEmpty())
      this->setToolButtonStyle(Qt::ToolButtonIconOnly);
    else
      this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
      //    resize(QSize(164, 164));
#ifdef WIN32
    QByteArray bA;
    bA.resize(1024);
    GetSystemDirectoryA(bA.data(), bA.size());
    _d->m_shell = bA + QString("\\cmd.exe");
    _d->preArgs << QString("/C");
#endif
    connect(this, &AutoButton::clicked, this, &AutoButton::actionButtonClicked);
    connect(&_d->m_process, &QProcess::readyReadStandardOutput, this,
            &AutoButton::processStdout);
    connect(&_d->m_process, &QProcess::readyReadStandardError, this,
            &AutoButton::processStderr);
    connect(&_d->m_process, &QProcess::started, this,
            &AutoButton::processStarted);
    connect(&_d->m_process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                &QProcess::finished),
            this, &AutoButton::processFinished);
    connect(&_d->m_process, &QProcess::errorOccurred, this,
            &AutoButton::processError);
  }

  struct Private {
    QProcess m_process;
    int disableTimerId;
    int checkTimerId;
    bool is_confirm;  // требуется подтверждение
    bool is_start;    // задан стартовый скрипт
    bool is_stop;     // задан стоповый скрипт
    bool is_detach;
    int killTimeout;
    int disableTimeout;
    QString m_shell;
    QStringList preArgs;
    QString startFn;
    QStringList startArgs;
    QString detachedFn;
    QStringList detachedArgs;
    QString confirmText;
    QString infoText;
    QString dir;
    QString styleSheetNormal;
    QString styleSheetPressed;
    Private()
        : m_process(),
          disableTimerId(NoTimerId),
          checkTimerId(NoTimerId),
          is_confirm(false),
          is_start(false),
          is_stop(false),
          is_detach(false),
          killTimeout(100),
          disableTimeout(1000),
          m_shell("/bin/bash"),
          preArgs(),
          startFn(),
          startArgs(),
          detachedFn(),
          detachedArgs(),
          confirmText(),
          infoText(),
          dir(),
          styleSheetNormal(),
          styleSheetPressed{"*{background:PaleGreen;}"} {}
  };
  Private *_d;
};
