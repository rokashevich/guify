TEMPLATE = app
TARGET = xxdialog

CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += \
  -L /opt/warehouse/externals/fltk/ubuntu1604/include \
  -L /opt/warehouse/externals/boost/ubuntu1604/include
LIBS += \
  -L /opt/warehouse/externals/fltk/ubuntu1604/lib \
  -lfltk -lfltk_forms -lfltk_gl -lfltk_images -lXext -lX11 -lXinerama -lXcursor \
  -lXrender -lXft -lXfixes -lfontconfig -lm -ldl

DEFINES += SOURCES_VERSION=\\\"$$(SVN_REVISION)\\\"

SOURCES +=  \
    main.cpp

