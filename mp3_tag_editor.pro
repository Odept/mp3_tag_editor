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
	textedit.cpp \
	job_file.cpp \
	mp3.cpp

HEADERS += window.h \
	External/inc/genre.h \
	External/inc/id3v1.h \
	External/inc/id3v2.h \
	External/inc/mpeg.h \
	textedit.h \
	job_file.h \
	mp3.h \
	error.h

FORMS += window.ui

LIBS += $$PWD/External/lib/tag.a \
	$$PWD/External/lib/mpeg.a
macx {
	LIBS += -liconv
}
