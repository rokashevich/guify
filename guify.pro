QT += core gui widgets xml svg svgwidgets
CONFIG += c++17
LIBS += -lpthread
SOURCES = $$files(*.cpp, true)
HEADERS = $$files(*.hpp, true)
