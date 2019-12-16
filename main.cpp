#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <utility>
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_draw.H>

class Item {
public:
  virtual ~Item() {}
  virtual std::string execute() = 0;
};

class ItemInput : public Item
{
public:
  Fl_Box* description;
  Fl_Input* field;
  virtual std::string execute() { return std::string(field->value()); }
};

class ItemCheck : public Item
{
public:
  Fl_Box* description;
  Fl_Input* field;
  virtual std::string execute() { return std::string(field->value()); }
};

class ItemRadio : public Item
{
public:
  Fl_Box* description;
  Fl_Input* field;
  virtual std::string execute() { return std::string(field->value()); }
};

class HorizontalLine: public Fl_Widget {
public:
  HorizontalLine(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H) {}
  void draw() {
    fl_color(0,0,0);
    fl_line_style(FL_SOLID, 1);
    fl_line(0,150,300,150);
  }
};

class Cfg {
public:
  Cfg():
    title_("xxdialog") {
  }
  bool Init(int argc, char **argv) {
    std::vector<std::string> possible_args = {"-T", "-I", "-C", "-R"};
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
        //std::cout << std::endl << "new sentence: ";
        std::vector<std::string> sentense = {arg};
        for (++i; i < argc; ++i) {
          arg = std::string(argv[i]);
          if (std::any_of(possible_args.begin(), possible_args.end(),
                          [=](std::string i){return i==arg;})) {
            --i;
            break;
          }
          //std::cout << arg << " ";
          sentense.push_back(arg);
        }
        sentenses_.push_back(sentense);
      }
    }
    widest_label_width_ = 0;
    for (const std::vector<std::string>&sentense : sentenses_) {
      std::string label = sentense.at(1);
      int label_width = static_cast<int>(label.length());
      if (label_width > widest_label_width_)
        widest_label_width_ = label_width;
    }

    return true;
  }
  std::string title() {return title_;}
  std::vector<std::vector<std::string> > sentenses() {return sentenses_;}

private:
  std::string title_;
  std::vector<std::vector<std::string> > sentenses_;
  int widest_label_width_;
};


class Panel: public Fl_Group {
public:
  Panel(int y, int width, int height, std::string title)
    : Fl_Group(0,y,width,height), title_offset_(0) {
    this->copy_label(title.c_str());
    this->box(FL_THIN_UP_BOX);
    this->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
    const int title_to_contents_width_ratio = 5; /// Для красоты.
    title_offset_ = static_cast<int>(w()/title_to_contents_width_ratio);
    margin_ = height / 5;
    // Наследуя этот класс не забываем добавить end() в конце конструктора!
  }
  virtual std::string result() = 0;
  int Height() {return h();}
  int TitleOffset() {return title_offset_;}
  int RemainingWidth() {return w() - title_offset_;}
  int Margin() {return margin_;}
private:
  int margin_;
  int title_offset_;
};


class Input: public Panel {
public:
  Input(int y, int width, int height, std::string label)
    : Panel(y, width, height, label) {
    input_ = new Fl_Input(TitleOffset(),
                          y+Margin(),
                          width-TitleOffset()-2*Margin()-Fl::scrollbar_size(),
                          Height()-2*Margin());
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
  Z(int y, int width, int height, std::string label,
    std::vector<std::string> options)
      : Panel(y, width, height, label) {
    int number_of_options = options.size();
    int option_width = RemainingWidth() / number_of_options;
    int option_x_offset = TitleOffset();
    for(auto const& option: options) {
      T* button = new T(
            option_x_offset, y, option_width, height);
      button->copy_label(option.c_str());
      option_x_offset += option_width;
      buttons.push_back(button);
    }
    end();
  }
  std::string result() {
    return "";
  }
private:
  std::vector<T*> buttons;
};



class Radio: Panel {
public:
};

class Xxdialog {
public:
  Xxdialog(std::string title, std::vector<std::vector<std::string> > sentenses)
    : title_(title), sentenses_(sentenses) {
    Fl_Window * window_ = new Fl_Window(0, 0, 0, 0, title_.data());
    Fl_Scroll * scroll = new Fl_Scroll(0, 0, 0, 0);
    scroll->type(Fl_Scroll::VERTICAL);

    int screen_num = 0;
    Fl::screen_work_area(work_area_x_, work_area_y_, work_area_w_, work_area_h_, screen_num);

    fl_font(FL_HELVETICA, FL_NORMAL_SIZE);
    double width_for_80_columns_ = fl_width('W')*80;
    font_height_ = fl_height();
    int panel_height = font_height_ * 3;

    // Изначально ширину окна рассчитваем, чтобы был вмещал 80 символов,
    // и в высоту чтобы сохранялась пропорциональность экрана.
    int window_w = static_cast<int>(width_for_80_columns_);
    int window_h = static_cast<int>(window_w * work_area_h_ / work_area_w_);

    int cumulative_height = 0;
    for (const std::vector<std::string>&sentense : sentenses_) {
      Panel* panel = nullptr;
      std::string key = sentense.at(0);
      std::string panel_label = sentense.at(1);
      std::vector<std::string> options;
      options.assign(sentense.begin()+2, sentense.end());

      if (key == "-I") {
        panel = new Input(cumulative_height, window_w, panel_height, panel_label);
      } else if (key == "-C") {
        panel = new Z<Fl_Check_Button>(cumulative_height, window_w, panel_height, panel_label, options);
      } else if (key == "-R") {
        panel = new Z<Fl_Radio_Round_Button>(cumulative_height, window_w, panel_height, panel_label, options);
      }
      if (panel) {
        scroll->add(panel);
        panels.push_back(panel);
        cumulative_height += panel->Height();
      }
    }

    int ok_cancel_width = window_w / 2;
    Fl_Return_Button* ok = new Fl_Return_Button(0,cumulative_height,ok_cancel_width,panel_height,"OK");
    ok->color(0x66CC6600);
    Fl_Button* cancel = new Fl_Button(ok_cancel_width,cumulative_height,ok_cancel_width,panel_height,"Cancel");
    auto fun=+[](Fl_Widget *w, void *data){
      std::cout << "cancel pressed" << std::endl;
    };
    cancel->callback(fun);
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
    scroll->resize(0, 0, window_w, window_h);
    window_->show();
  }

  int Run() {
    return Fl::run();
  }
private:
  int work_area_x_, work_area_y_, work_area_w_, work_area_h_;
  std::string title_;
  std::vector<std::vector<std::string> > sentenses_;
  int font_height_;
  std::vector<Panel*> panels;
};

int usage() {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "    -T title" << std::endl;
  std::cerr << "    -I description" << std::endl;
  std::cerr << "    -C description checkboxes" << std::endl;
  std::cerr << "    -R description radiobuttons" << std::endl;
  return 1;
}

void ok_callback(Fl_Widget *w, void *data) {
  std::vector<Item*> items = *reinterpret_cast<std::vector<Item*>*>(data);
  for (Item* i : items) {
    ItemInput* item = dynamic_cast<ItemInput*>(i);
    std::cout << item->field->value() << std::endl;
  }
  exit(0);
}

int main(int argc, char **argv) {
  Cfg* cfg = new Cfg();
  if (!cfg->Init(argc, argv)) {
    exit(1);
  }
  Xxdialog* xxdialog = new Xxdialog(cfg->title(), cfg->sentenses());
  return xxdialog->Run();





  //ok->callback(ok_callback, &items);

}
