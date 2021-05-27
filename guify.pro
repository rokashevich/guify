QT += core gui widgets
CONFIG += c++17
LIBS += -lpthread
SOURCES = $$files(*.cpp, true)
HEADERS = $$files(*.hpp, true)
