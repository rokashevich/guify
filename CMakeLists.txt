# Установка всех необходимых зависимостей в kde neon (18.04):
# apt install libfltk1.3-dev fluid libboost-all-dev libsm-dev libxinerama-dev libxcursor-dev libxft-dev libcairo2-dev
CMAKE_MINIMUM_REQUIRED(VERSION 3.5.1)
SET (CMAKE_CXX_STANDARD 11)
PROJECT(fltkdialog)
FIND_PACKAGE(FLTK REQUIRED)
ADD_EXECUTABLE(fltkdialog main.cpp cfg.cpp helpers.hpp server.cpp server.hpp)
TARGET_LINK_LIBRARIES(fltkdialog fltk_STATIC)
