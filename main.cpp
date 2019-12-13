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

class Panel: public Fl_Box {
public:
    Panel(int y, int width, int height, const char * title)
            : Fl_Box(FL_THIN_UP_BOX,0,y,width,height,title), title_offset_(0) {
        this->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
        const int title_to_contents_width_ratio = 5; /// Для красоты.
        title_offset_ = static_cast<int>(w()/title_to_contents_width_ratio);
        margin_ = height / 5;
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
    Input(int y, int width, int height, const char * title)
            : Panel(y, width, height, title) {
        input_ = new Fl_Input(TitleOffset()+Margin(),
                              y+Margin(),
                              width-TitleOffset()-2*Margin(),
                              Height()-2*Margin());
    }
    std::string result() {
        return std::string(input_->value());
    }
private:
    Fl_Input* input_;
};

class Check: Panel {
public:
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

        /** Изначально ширину окна рассчитваем, чтобы был вмещал 80 символов,
         * и в высоту чтобы сохранялась пропорциональность экрана.
         */
        int window_w = static_cast<int>(width_for_80_columns_);
        int window_h = static_cast<int>(window_w * work_area_h_ / work_area_w_);

        int y_offset = 0;
        for (const std::vector<std::string>&sentense : sentenses_) {
            Panel* panel = nullptr;
            const char * label = sentense.at(1).c_str();
            std::string key = sentense.at(0);
            int panel_height = font_height_ * 3;
            if (key == "-I") {
                panel = new Input(y_offset, window_w, panel_height, label);
            } else if (key == "-C") {

            } /*else if (key == "-R") {

            }*/
            if (panel) {
                scroll->add(panel);
                panels.push_back(panel);
                y_offset += panel->Height();
            }
        }

        /** Если суммарная высота всех панелей и кнопки меньше рассчитаной
         * высоты окна, то уменьшаем высоту окна, по сути, для красоты.
         */
        int real_height = y_offset + font_height_;
        if (real_height < window_h)
            window_h = real_height;

        /** Если после подгонки размера окна под содержимое оно всё равно больше
         * рабочей области монитора, делаем окно программы размером с рабочую
         * область, опять же, для красоты.
         */
        if (window_w > work_area_w_ || window_h > work_area_h_) {
            window_w = work_area_w_;
            window_h = work_area_h_;
        }

        /** Подбираем координаты верхнего-левого угла окна, чтобы оно рисовалось
         * точно по середине. Для окна, занимающиего всю рабочую область это
         * будет 0,0. Для красоты.
          */
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


    std::vector<Item*> items;
    std::vector<std::string> possible_args = {"-T", "-I", "-C", "-R"};


    // Рассчитываем размеры под размер А5 - 148х210мм, и высота строки 10мм
    int window_width_mm = 210;
    int window_height_mm = 148;
    int screen_n = 0; // если несколько мониторов считаем по первому
    float inch = 25.4f; // миллиметров в дюйме
    float dpi_hor, dpi_vert;
    Fl::screen_dpi(dpi_hor, dpi_vert, screen_n);
    int window_width_px = static_cast<int>(window_width_mm * dpi_hor / inch);
    int window_height_px = static_cast<int>(window_height_mm * dpi_vert / inch);
    int screen_x, screen_y, screen_w, screen_h;
    Fl::screen_work_area(screen_x, screen_y, screen_w, screen_h, screen_n);
    if (window_width_px > screen_w || window_height_px > screen_h) {
        window_width_px = screen_w;
        window_height_px = screen_h;
    }
    int window_x = static_cast<int>((screen_w - window_width_px) / 2);
    int window_y = static_cast<int>((screen_h - window_height_px) / 2);


    Fl_Window *window = new Fl_Window(window_x, window_y, window_width_px,window_height_px);
    window->size_range(window_width_px,window_height_px);

    Fl_Scroll *scroll = new Fl_Scroll(0,0,500,450);
    scroll->type(Fl_Scroll::VERTICAL);
    scroll->end();
    for (int i = 1; i < argc; ++i) {
        std::string arg = std::string(argv[i]);
        if (arg == "-T") {
            if (argc > i+1
                    && !std::any_of(possible_args.begin(), possible_args.end(),
                                    [=](std::string k){return k==argv[i+1];})) {
                window->label(argv[++i]);
                continue;
            } else
                exit(usage());
        }
        if (arg == "-I") {
            if (argc > i+1
                    && !std::any_of(possible_args.begin(), possible_args.end(),
                                    [=](std::string k){return k==argv[i+1];})) {
                int y = 100*static_cast<int>(items.size());
                ItemInput* input = new ItemInput;
                input->description = new Fl_Box(FL_ENGRAVED_BOX,0,y,500,100, argv[++i]);
                input->description->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP);
                input->field = new Fl_Input(input->description->x()+5,
                                            input->description->y()+50,
                                            input->description->w()-10,45);
                items.push_back(input);
                scroll->add(input->description);
                scroll->add(input->field);
                continue;
            } else
                exit(usage());
        }
        if (arg == "-C"
                && !std::any_of(possible_args.begin(), possible_args.end(),
                                [=](std::string k){return k==argv[i+1];})) {
            std::string checkboxes_description;
            std::vector<std::string> checkboxes_words;
            if (argc > i+2) {
                ItemCheck* check = new ItemCheck;
                check->description = new Fl_Box(FL_ENGRAVED_BOX,0,25*static_cast<int>(items.size()),500,25, argv[++i]);
                scroll->add(check->description);
                for (int j = i; j < argc; ++j) {
                    const std::string search = std::string(argv[j]);
                    if (std::any_of(possible_args.begin(), possible_args.end(),
                                    [=](std::string i){return i==search;})) {
                        break;
                    }
                    std::string word = std::string(argv[i++]);
                    checkboxes_words.push_back(word);
                    std::cout << "words: " << word << std::endl;
                }
            } else {
                exit(usage());
            }
        }
        if (arg == "-R"
                && !std::any_of(possible_args.begin(), possible_args.end(),
                                [=](std::string k){return k==argv[i+1];})) {
            std::string radiobuttons_description;
            std::vector<std::string> radiobuttons_items;
            if (argc > i+3) {
                ItemRadio* radio = new ItemRadio;
                radio->description = new Fl_Box(FL_ENGRAVED_BOX,5,50*static_cast<int>(items.size()),300,50, argv[++i]);
                radio->description->align(FL_ALIGN_BOTTOM);
                Fl_Group *g = new Fl_Group(0,0,250,250);
                g->add(radio->description);
                for (int j = i; j < argc; ++j) {
                    const std::string search = std::string(argv[j]);
                    if (std::any_of(possible_args.begin(), possible_args.end(),
                                    [=](std::string i){return i==search;})) {
                        break;
                    }
                    std::string word = std::string(argv[j]);
                    radiobuttons_items.push_back(word);
                    Fl_Radio_Round_Button* rb = new Fl_Radio_Round_Button(0,25*(j-i)+50,200,25,argv[j]);
                    g->add(rb);
                    std::cout << i << "words: " << word << std::endl;
                }
                g->end();
                std::cout << g->children()  << " " << std::endl;//<< dynamic_cast<Fl_Radio_Round_Button*>(g->child(0))->value() << std::endl;
            } else {
                exit(usage());
            }
        }
    }

    HorizontalLine* lines = new HorizontalLine(0,0,300,300);
    window->add(lines);

    //Fl_Return_Button* ok = new Fl_Return_Button(0,200,250,50,"OK");
    //ok->callback(ok_callback, &items);

    window->resizable(scroll);
    window->show();
    return Fl::run();
}
