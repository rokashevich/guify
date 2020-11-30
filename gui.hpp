#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <string>
#include <vector>

#include "cfg.hpp"
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
  int Height() { return h(); }
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
  std::string result() { return std::string(input_->value()); }

 private:
  Fl_Input* input_;
};

template <class T>
class Z : public Panel {
 public:
  Z(int y, int panel_name_width, int width, int height, std::string label,
    std::vector<std::string> options, int option_width)
      : Panel(y, width, height, label) {
    int value = 0;
    for (auto const& option : options) {
      T* button = new T(panel_name_width, y, option_width, height);
      button->copy_label(option.c_str());
      panel_name_width += option_width;
      button->value(!value++);
      buttons.push_back(button);
    }
    end();
  }
  std::string result() {
    std::vector<std::string> chosen_values;
    for (T* button : buttons) {
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
class Gui {
  Fl_Window* window_;
  Cfg* cfg_;
  int work_area_x_, work_area_y_, work_area_w_, work_area_h_;
  std::vector<Panel*> panels_;

 public:
  void ToDialog() {
    Fl_Scroll* scroll = new Fl_Scroll(0, 0, 0, 0);
    scroll->type(Fl_Scroll::VERTICAL);
    scroll->begin();

    // В fltk размер шрифта фиксирован.
    // Масштабируем его в зависимости от dpi монитора.
    // Дополнительно делаем шрифт в два раза больше нормального.
    const int default_dpi = 72;
    float h, v;  // v - dpi по вертикали - не понадобится.
    Fl::screen_dpi(h, v);
    FL_NORMAL_SIZE = h / default_dpi * FL_NORMAL_SIZE;

    fl_font(FL_BOLD, FL_NORMAL_SIZE);

    int panel_height = fl_height() * 2;
    int panel_name_width =
        static_cast<int>(fl_width((cfg_->LongestPanelName()).data())) +
        Fl::scrollbar_size();
    int option_width =
        static_cast<int>(fl_width(cfg_->LongestOptionName().data()));
    int contents_width =
        option_width * cfg_->MaxOptinsCount() + Fl::scrollbar_size();

    // Ширину окна рассчитваем, чтобы влезали названия панелей и их содержимое.
    // А высота окна чтобы была в 75% от доступной. В дальнейшем высота будет
    // пересчитана в зависимости от суммарной высоты всех панелей, и, либо
    // уменьшена, либо появится скрол.
    int window_w = panel_name_width + contents_width;
    int window_h = static_cast<int>(work_area_h_ * 0.75);

    int cumulative_height = 0;
    for (const std::vector<std::string>& sentense : cfg_->Sentenses()) {
      Panel* panel = nullptr;
      std::string key = sentense.at(0);
      std::string panel_label = sentense.at(1);
      std::vector<std::string> options;
      options.assign(sentense.begin() + 2, sentense.end());

      if (key == "-I") {
        panel = new Input(cumulative_height, panel_name_width, window_w,
                          panel_height, panel_label, options.at(0));
      } else if (key == "-C") {
        panel = new Z<Fl_Check_Button>(cumulative_height, panel_name_width,
                                       window_w, panel_height, panel_label,
                                       options, option_width);
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
    if (cumulative_height < window_h) window_h = cumulative_height;

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
    int window_x =
        static_cast<int>((work_area_w_ - window_w) / 2 + work_area_x_);
    int window_y =
        static_cast<int>((work_area_h_ - window_h) / 2 + work_area_y_);

    window_->resize(window_x, window_y, window_w, window_h);
    scroll->resize(0, 0, window_w, window_h - panel_height);

    // После всех паналей снизу окна программы рисуем кнопки OK и Cancel,
    // каждая в пол-окна шириной.
    int ok_cancel_width = window_w / 2;

    // Снизу-слева кнопка ОК.
    Fl_Return_Button* ok = new Fl_Return_Button(
        0, window_h - panel_height, ok_cancel_width, panel_height, "OK");

    // По кнопке OK построчно выводим значения панелей и выходим по exit(0).
    ok->callback(
        [](Fl_Widget*, void* x) {
          std::vector<Panel*>* panels = static_cast<std::vector<Panel*>*>(x);
          std::vector<std::string> results;
          for (Panel* a : *panels) {
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
        },
        static_cast<void*>(&panels_));

    // Снизу-справа кнопка Cancel.
    Fl_Button* cancel = new Fl_Button(ok_cancel_width, window_h - panel_height,
                                      ok_cancel_width, panel_height, "Cancel");

    // По кнопке Cancel программа просто выходит по exit(1).
    cancel->callback([](Fl_Widget*, void*) { exit(1); });
  }
  void ToProcess() {
    Fl_Text_Display::Style_Table_Entry* stable =
        new Fl_Text_Display::Style_Table_Entry[4];
    stable[0] = {FL_RED, FL_HELVETICA, 18};          // A - Red
    stable[1] = {FL_DARK_YELLOW, FL_HELVETICA, 18};  // B - Yellow
    stable[2] = {FL_DARK_GREEN, FL_HELVETICA, 18};   // C - Green
    stable[3] = {FL_BLUE, FL_HELVETICA, 18};         // D - Blue);

    Fl_Text_Display* disp =
        new Fl_Text_Display(20, 20, 640 - 40, 480 - 40, "Display");
    Fl_Text_Buffer* tbuff = new Fl_Text_Buffer();  // text buffer
    Fl_Text_Buffer* sbuff = new Fl_Text_Buffer();  // style buffer
    disp->buffer(tbuff);
    disp->highlight_data(sbuff, stable, 4, 'A', 0, 0);
    tbuff->text(
        "Red Line 1\nYel Line 2\nGrn Line 3\nBlu Line 4\n"
        "Red Line 5\nYel Line 6\nGrn Line 7\nBlu Line 8\n");
    sbuff->text("AAAAAAAAAA\nBBBBBBBBBB\n");
    window_->resizable(*disp);
  }
  Gui(Cfg* cfg) {
    window_ = new Fl_Window(10, 10, 640, 480, cfg->Title().data());
    Fl::screen_work_area(work_area_x_, work_area_y_, work_area_w_, work_area_h_,
                         0);
    if (cfg->Sentenses().at(0).at(0) == "-P")
      ToProcess();
    else
      ToDialog();

    window_->show();
  }

  int Run() { return Fl::run(); }

  void NumInstancesChanged(int num_instances, int own_num) {
    std::cout << "NumInstancesChanged " << num_instances << " " << own_num
              << std::endl;
  }
};
