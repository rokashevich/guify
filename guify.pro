QT += core gui widgets
CONFIG += c++14
LIBS += -lpthread
SOURCES = $$files(*.cpp, true)
HEADERS = $$files(*.hpp, true)
