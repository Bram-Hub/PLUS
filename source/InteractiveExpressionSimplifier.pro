#-------------------------------------------------
#
# Project created by QtCreator 2013-04-30T21:19:49
#
#-------------------------------------------------

QT       += core gui

TARGET = PropositionalLogicUtilitySuite
TEMPLATE = app
RC_FILE = icon.rc
SOURCES += main.cpp \
    mainwindow.cpp \
    Expression.cpp \
    BooleanParser.cpp \
    expressionserializer.cpp

HEADERS  += mainwindow.h \
    Expression.h \
    BooleanParser.h \
    expressionserializer.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    icon.rc \
    icon.ico





