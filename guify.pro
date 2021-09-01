QT += core gui widgets xml svg svgwidgets
CONFIG += c++17
LIBS += -lpthread
# https://doc.qt.io/qt-6/qmake-function-reference.html
SOURCES = $$files(*.cpp, true)
HEADERS = $$files(*.hpp, true)

VERSION=$$system(git show -s --date=format:"%Y%m%d" --format=%cd-%h 2>/dev/null || svn info|grep \"Last Changed Rev: \"|cut -d\" \" -f4)
message(VERSION=\"$$VERSION\")
DEFINES += VERSION='\\\"$$VERSION\\\"'
OTHER_FILES += $$files(*.qss, true)
OTHER_FILES += $$files(*.svg, true)
OTHER_FILES += $$files(*.sh, true)
