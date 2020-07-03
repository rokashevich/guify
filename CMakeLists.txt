# Установка всех необходимых зависимостей в kde neon (18.04):
# apt install libfltk1.3-dev fluid libboost-all-dev libsm-dev libxinerama-dev libxcursor-dev libxft-dev libcairo2-dev
CMAKE_MINIMUM_REQUIRED(VERSION 3.5.1)
SET (CMAKE_CXX_STANDARD 11)
PROJECT(xxdialog)
FIND_PACKAGE(FLTK REQUIRED)
ADD_EXECUTABLE(xxdialog main.cpp)
TARGET_LINK_LIBRARIES(xxdialog fltk_STATIC)
