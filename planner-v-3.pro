TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    planner.c \
    scheduler.c \
    sysdep.c

HEADERS += planner.h \
    scheduler.h \
    sysdep.h \
    task.h
