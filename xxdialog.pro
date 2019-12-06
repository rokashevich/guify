TEMPLATE = app
TARGET = xxdialog

CONFIG -= qt

INCLUDEPATH += \
  -L /opt/warehouse/externals/fltk/ubuntu1604/include
LIBS += \
  -L /opt/warehouse/externals/fltk/ubuntu1604/lib \
  -lfltk -lfltk_forms -lfltk_gl -lfltk_images -lXext -lX11 -lXinerama -lXcursor \
  -lXrender -lXft -lXfixes -lfontconfig -lm -ldl

DEFINES += SOURCES_VERSION=\\\"$$(SVN_REVISION)\\\"

SOURCES +=  \
    main.cpp

