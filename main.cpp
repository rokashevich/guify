#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

// Назначение класса - распарсить аргументы командной строки.
// Помимо проверки введённых параметров на правильность,
// производится поиск:
//   - самого длинного названия панели (левый столбик);
//   - самый длинный вариант;
//   - наибольшее кол-во вариантов в панели.
// Эти поля будут нужны для красивой компоновки виджетов в классе Xxdialog.
class Cfg {
public:
  bool Init(int argc, char **argv) {
    std::vector<std::string> possible_args = {"-T", "-I", "-C", "-R", "-D"};
    for (int i = 1; i < argc; ++i) {
      std::string arg = std::string(argv[i]);
      if (arg == "-T") {
        if (argc > i+1
            && !std::any_of(possible_args.begin(), possible_args.end(),
                            [=](std::string k){return k==argv[i+1];})) {
          title_ = argv[++i];
          continue;
        } else {
          return false;
        }
      }
      else if (std::any_of(possible_args.begin(), possible_args.end(),
                           [=](std::string i){return i==arg;})) {
        std::vector<std::string> sentense = {arg};
        for (++i; i < argc; ++i) {
          arg = std::string(argv[i]);
          if (std::any_of(possible_args.begin(), possible_args.end(),
                          [=](std::string i){return i==arg;})) {
            --i;
            break;
          }
          sentense.push_back(arg);
        }
        sentenses_.push_back(sentense);
      }
    }

    for (const std::vector<std::string>&sentense : sentenses_) {
      // Ищем самое длинное название панели.
      std::string label = sentense.at(1);
      if (label.length() > longest_panel_name_.length())
        longest_panel_name_ = label;

      // Ищем самый длинный вариант и набольшее кол-во вариантов.
      int options_in_this_sentense = 0;
      std::vector<std::string> options = sentense;
      options.erase(options.begin(), options.begin()+2);
      for (const std::string& option: options) {
        ++options_in_this_sentense;
        if (option.length() > longest_option_name_.length()) {
          longest_option_name_ = option;
        }
      }
      if (options_in_this_sentense > max_options_count_)
        max_options_count_ = options_in_this_sentense;
    }

    return sentenses_.size() || longest_panel_name_.size() ||
           longest_option_name_.size() || max_options_count_;
  }
  std::string Title() {return title_;}
  std::vector<std::vector<std::string> > Sentenses() {return sentenses_;}
  std::string LongestPanelName() {return longest_panel_name_;}
  std::string LongestOptionName() {return longest_option_name_;}
  int MaxOptinsCount() {return max_options_count_;}

private:
  std::string title_ {"xxdialog"};
  std::vector<std::vector<std::string> > sentenses_;
  std::string longest_panel_name_; // Самое длинное название панели.
  std::string longest_option_name_ {""}; // Самый длинный ваирант.
  int max_options_count_ {0}; // Наибольшее кол-во вариантов в одной панели.
};

// Базовый класс панели.
class Panel : public Fl_Group {
 public:
  Panel(int y, int width, int height, std::string title)
      : Fl_Group(0, y, width, height) {
    this->copy_label(title.c_str());
    this->box(FL_THIN_UP_BOX);
    this->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
    // Наследуя этот класс не забываем добавить end() в конце конструктора!
  }
  virtual std::string result() = 0;
  int Height() {return h();}
};

class Input : public Panel {
 public:
  Input(int y, int panel_name_width, int width, int height,
        const std::string label, const std::string default_value)
      : Panel(y, width, height, label) {
    input_ =
        new Fl_Input(panel_name_width, y,
                     width - panel_name_width - Fl::scrollbar_size(), Height());
    input_->value(default_value.c_str());
    end();
  }
  std::string result() {
    return std::string(input_->value());
  }
private:
  Fl_Input* input_;
};

template <class T>
class Z: public Panel {
public:
  Z(int y, int panel_name_width, int width, int height, std::string label,
    std::vector<std::string> options, int option_width)
      : Panel(y, width, height, label) {
    int value = 0;
    for (auto const& option : options) {
      T* button = new T(
            panel_name_width, y, option_width, height);
      button->copy_label(option.c_str());
      panel_name_width += option_width;
      button->value(!value++);
      buttons.push_back(button);
    }
    end();
  }
  std::string result() {
    std::vector<std::string> chosen_values;
    for (T* button: buttons) {
      if (button->value()) {
        const std::string value = button->label();
        chosen_values.push_back(value);
      }
    }
    return boost::algorithm::join(chosen_values, " ");
  }
private:
  std::vector<T*> buttons;
};

class DirectoryChooser : public Panel {
 public:
  DirectoryChooser(int y, int panel_name_width, int width, int height,
                   std::string label, const std::vector<std::string>& options)
      : Panel(y, width, height, label) {
    browse_button_ =
        new Fl_Button(panel_name_width, y + fl_height() / 4,
                      width - panel_name_width - Fl::scrollbar_size(),
                      Height() - fl_height() / 2);
    end();
    this->result(options.at(0));
    browse_button_->box(FL_BORDER_BOX);
    browse_button_->color(FL_DARK1);
    browse_button_->callback(
        [](Fl_Widget*, void* x) {
          DirectoryChooser* dc = static_cast<DirectoryChooser*>(x);
          Fl_File_Chooser chooser(dc->result().data(), "*",
                                  Fl_File_Chooser::DIRECTORY,
                                  "Choose directory");
          chooser.preview(0);
          chooser.show();
          while (chooser.shown()) Fl::wait();
          if (chooser.value() == NULL) return;
          dc->result(chooser.value());
        },
        this);
  }
  std::string result() { return chosen_directory_; }
  void result(const std::string value) {
    chosen_directory_ = value;
    browse_button_->label(chosen_directory_.c_str());
  }

 private:
  Fl_Button* browse_button_;
  std::string chosen_directory_;
};

class Xxdialog {
 public:
  Xxdialog(Cfg* cfg) {
    Fl_Window * window_ = new Fl_Window(0, 0, 0, 0, cfg->Title().data());
    Fl_Scroll * scroll = new Fl_Scroll(0, 0, 0, 0);
    scroll->type(Fl_Scroll::VERTICAL);
    scroll->begin();

    int screen_num = 0;
    Fl::screen_work_area(work_area_x_, work_area_y_, work_area_w_, work_area_h_,
                         screen_num);

    // В fltk размер шрифта фиксирован.
    // Масштабируем его в зависимости от dpi монитора.
    // Дополнительно делаем шрифт в два раза больше нормального.
    const int default_dpi = 72;
    float h, v;  // v - dpi по вертикали - не понадобится.
    Fl::screen_dpi(h, v);
    FL_NORMAL_SIZE = h / default_dpi * FL_NORMAL_SIZE;

    fl_font(FL_BOLD, FL_NORMAL_SIZE);

    int panel_height = fl_height() * 2;
    int panel_name_width = static_cast<int>(
          fl_width((cfg->LongestPanelName()).data())) + Fl::scrollbar_size();
    int option_width =
        static_cast<int>(fl_width(cfg->LongestOptionName().data()));
    int contents_width = option_width * cfg->MaxOptinsCount() + Fl::scrollbar_size();

    // Ширину окна рассчитваем, чтобы влезали названия панелей и их содержимое.
    // А высота окна чтобы была в 75% от доступной. В дальнейшем высота будет
    // пересчитана в зависимости от суммарной высоты всех панелей, и, либо
    // уменьшена, либо появится скрол.
    int window_w = panel_name_width + contents_width;
    int window_h = static_cast<int>(work_area_h_ * 0.75);

    int cumulative_height = 0;
    for (const std::vector<std::string>&sentense : cfg->Sentenses()) {
      Panel* panel = nullptr;
      std::string key = sentense.at(0);
      std::string panel_label = sentense.at(1);
      std::vector<std::string> options;
      options.assign(sentense.begin()+2, sentense.end());

      if (key == "-I") {
        panel = new Input(cumulative_height, panel_name_width, window_w,
                          panel_height, panel_label, options.at(0));
      } else if (key == "-C") {
        panel = new Z<Fl_Check_Button>(
              cumulative_height, panel_name_width, window_w, panel_height,
              panel_label, options, option_width);
      } else if (key == "-R") {
        panel = new Z<Fl_Radio_Round_Button>(
              cumulative_height, panel_name_width, window_w, panel_height,
              panel_label, options, option_width);
      } else if (key == "-D") {
        panel =
            new DirectoryChooser(cumulative_height, panel_name_width, window_w,
                                 panel_height, panel_label, options);
      }
      if (panel) {
        scroll->add(panel);
        panels_.push_back(panel);
        cumulative_height += panel->Height();
      }
    }
    scroll->end();

    cumulative_height += panel_height;

    // Если суммарная высота всех панелей и кнопки меньше рассчитаной
    // высоты окна, то уменьшаем высоту окна, по сути, для красоты.
    if (cumulative_height < window_h)
      window_h = cumulative_height;

    // Если после подгонки размера окна под содержимое оно всё равно больше
    // рабочей области монитора, делаем окно программы размером с рабочую
    // область, опять же, для красоты.
    if (window_w > work_area_w_ || window_h > work_area_h_) {
      window_w = work_area_w_;
      window_h = work_area_h_;
    }

    // Подбираем координаты верхнего-левого угла окна, чтобы оно рисовалось
    // точно по середине. Для окна, занимающиего всю рабочую область это
    // будет 0,0. Для красоты.
    int window_x = static_cast<int>((work_area_w_ - window_w) / 2 + work_area_x_);
    int window_y = static_cast<int>((work_area_h_ - window_h) / 2 + work_area_y_);

    window_->resize(window_x, window_y, window_w, window_h);
    scroll->resize(0, 0, window_w, window_h - panel_height);

    // После всех паналей снизу окна программы рисуем кнопки OK и Cancel,
    // каждая в пол-окна шириной.
    int ok_cancel_width = window_w / 2;

    // Снизу-слева кнопка ОК.
    Fl_Return_Button* ok = new Fl_Return_Button(0,window_h-panel_height,ok_cancel_width,panel_height,"OK");

    // По кнопке OK построчно выводим значения панелей и выходим по exit(0).
    ok->callback([](Fl_Widget *, void *x) {

      std::vector<Panel*>* panels = static_cast<std::vector<Panel*>*>(x);
      std::vector<std::string> results;
      for (Panel* a: *panels) {
        const std::string result =
            boost::replace_all_copy(a->result(), " ", "\\ ");
        if (result.length() == 0) {
          results.clear();
          break;
        }
        results.push_back(result);
      }
      if (results.size() > 0) {
        std::string result = boost::algorithm::join(results, " ");
        std::cout << result << std::endl;
        exit(0);
      }
    },static_cast<void*>(&panels_));

    // Снизу-справа кнопка Cancel.
    Fl_Button* cancel = new Fl_Button(ok_cancel_width,window_h-panel_height,ok_cancel_width,panel_height,"Cancel");

    // По кнопке Cancel программа просто выходит по exit(1).
    cancel->callback([](Fl_Widget*, void*) { exit(1); });

    window_->show();
  }

  int Run() {
    return Fl::run();
  }
private:
  Cfg* cfg;
  int work_area_x_, work_area_y_, work_area_w_, work_area_h_;
  std::vector<Panel*> panels_;
};

int usage() {
  // Принцип использования скобок:
  //   [для опциональных аргументов]
  //   <для обязательный оругментов>
  //   {для стандартных значений}
  //   (прочее)

  std::cerr << "Usage:" << std::endl;
  std::cerr << "    -T <title>" << std::endl;
  std::cerr << "    -I <input name> [default value]" << std::endl;
  std::cerr << "    -C <checkboxes name> <box 1 name> [other boxes]"
            << std::endl;
  std::cerr << "    -R <radio buttons name> <button 1 name> <button 2 name> "
               "[other buttons]"
            << std::endl;
  std::cerr << "    -D <directory selector name> [initial directory]"
            << std::endl;
  return 1;
}

int main(int argc, char **argv) {
  Cfg* cfg = new Cfg();
  if (!cfg->Init(argc, argv)) {
    exit(usage());
  }
  Xxdialog* xxdialog = new Xxdialog(cfg);
  return xxdialog->Run();
}
