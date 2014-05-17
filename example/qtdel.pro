# -------------------------------------------------
# Project created by QtCreator 2009-03-13T18:03:09
# -------------------------------------------------
QT		+= gui core widgets
TARGET = qtdel
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    delform.cpp \
    ../delaunay.c \
    ../predicates.c
HEADERS += mainwindow.h \
    delform.h \
    ../delaunay.h
FORMS += mainwindow.ui \
    delform.ui

QMAKE_CXXFLAGS += -DQT_INCLUDE_COMPAT
