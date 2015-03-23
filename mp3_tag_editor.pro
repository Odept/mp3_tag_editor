#-------------------------------------------------
#
# Project created by QtCreator 2015-03-19T19:24:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mp3_tag_editor
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
		window.cpp \
		tab_file.cpp \
		textedit.cpp

HEADERS  += window.h \
	External/inc/genre.h \
	External/inc/id3v1.h \
	External/inc/id3v2.h \
	textedit.h

FORMS    += window.ui

#LIBS += -liconv
LIBS += $$PWD/External/lib/id3v2.a

