#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
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
		window_title_("xxdialog") {
	}
	bool Init(int argc, char **argv) {
		std::vector<std::string> possible_args = {"-T", "-I", "-C", "-R"};
		for (int i = 1; i < argc; ++i) {
			std::string arg = std::string(argv[i]);
			if (arg == "-T") {
				if (argc > i+1
						&& !std::any_of(possible_args.begin(), possible_args.end(),
														[=](std::string k){return k==argv[i+1];})) {
					window_title_ = argv[++i];
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
					ordered_sentenses_.push_back(sentense);
			}
		}
		return true;
	}
	std::string window_title() {return window_title_;}

private:
	std::string window_title_;
	std::vector<std::vector<std::string> > ordered_sentenses_;
};

class Xxdialog {
public:
	Xxdialog() {
		// Рассчитываем размеры под размер А5 - 148х210мм, и высота строки 10мм
		int window_width_mm = 210;
		int window_height_mm = 148;
		int screen_n = 0; // если несколько мониторов считаем по первому
		float inch = 25.4f; // миллиметров в дюйме
		float dpi_hor, dpi_vert;
		Fl::screen_dpi(dpi_hor, dpi_vert, screen_n);
		window_w_ = static_cast<int>(window_width_mm * dpi_hor / inch);
		window_h_ = static_cast<int>(window_height_mm * dpi_vert / inch);
		int screen_x, screen_y, screen_w, screen_h;
		Fl::screen_work_area(screen_x, screen_y, screen_w, screen_h, screen_n);
		if (window_w_ > screen_w || window_h_ > screen_h) {
			window_w_ = screen_w;
			window_h_ = screen_h;
		}
		window_x_ = static_cast<int>((screen_w - window_w_) / 2);
		window_y_ = static_cast<int>((screen_h - window_h_) / 2);
		std::cout << window_x_ << " "<< window_y_ << " "<< window_w_ << " "<< window_h_ << std::endl;
	}

	int Run() {
		Fl_Window *window = new Fl_Window(window_x_, window_y_, window_w_, window_h_);
		window->show();
		return Fl::run();
	}
private:
	int window_x_, window_y_, window_w_, window_h_;
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
	Xxdialog* xxdialog = new Xxdialog;
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
