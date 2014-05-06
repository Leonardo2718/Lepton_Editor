#------------------------------------------------------------------------------
#
# Project: Lepton Editor - Created by QtCreator 2014-01-31T12:45:13
# File: main.cpp
# Author: Leonardo Banderali
# Created: January 31, 2014
# Last Modified: May 5, 2014
#
# Description:
#    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
#    flexible and extensible code editor which developers can easily customize to their
#    liking.
#
#    This is the qmake file which contains data about all other the project files.
#
# Copyright (C) 2014 Leonardo Banderali
#
# Usage Agreement:
#    This file is part of Lepton Editor
#
#    Lepton Editor is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    any later version.
#
#    Lepton Editor is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#------------------------------------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Lepton
TEMPLATE = app

CONFIG      += qscintilla2

SOURCES += main.cpp\
        mainwindow.cpp \
#    editor.cpp \
    editortabbar.cpp \
#    syntaxhighlighter.cpp \
    scintillaeditor.cpp

HEADERS  += mainwindow.h \
#    editor.h \
    editortabbar.h \
#    syntaxhighlighter.h \
    scintillaeditor.h

FORMS    += mainwindow.ui

unix|win32: LIBS += -lqscintilla2

#macx {
#    QMAKE_POST_LINK = install_name_tool -change libqscintilla2.11.dylib $$[QT_INSTALL_LIBS]/libqscintilla2.11.dylib $(TARGET)
#}
