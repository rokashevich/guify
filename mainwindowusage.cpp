#include "mainwindowusage.hpp"

#include <QTextEdit>

MainWindowUsage::MainWindowUsage() : MainWindow() {
  QTextEdit* te = new QTextEdit;
  te->setText(R"(bguify { D | P | B | O | M }
    D (for Dialog) options:
      I text_input_var_name [initial string];
      C checkbox_var_name option1 ...;
      R radio_var_name option1 option2 ...;
      D directory_var_name [initial directory]; default=~

    P (for Process) options:
      process_binary [arg1 ...]

    B (for progressbar)
    O (for OSD)

    M (for piped menu) options:
      openbox syntaxed pipe menu script | folder)");
  this->setCentralWidget(te);
}
