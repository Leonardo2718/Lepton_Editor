/*
Project: Lepton Editor
File: editortabbar.h
Author: Leonardo Banderali
Created: February 9, 2014
Last Modified: April 2, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the definition of the object used to produce the tabs for each
    editing window, 'EditorTabBar'.

Copyright (C) 2014 Leonardo Banderali

Usage Agreement:
    This file is part of Lepton Editor

    Lepton Editor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    Lepton Editor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EDITORTABBAR_H  //prevent the file from being included more than once
#define EDITORTABBAR_H

//include necessary files and libraries
#include <QTabWidget>
#include <QAction>
#include <QVector>
#include "editor.h"

class EditorTabBar : public QTabWidget
{
    Q_OBJECT

    public:
        explicit EditorTabBar(QWidget *parent = 0);
        /* -configure the widget */

        ~EditorTabBar();
        /* -dealocates (deletes) all the editor tabs stored in 'tabs' */

        QVector< Editor* > tabs;    //vector to store pointers to all the editor tabs

        int addTab(int index = -1);
        /*
        -instantiate a new instance of the 'Editor' class, add it to 'tabs', and create
         an actual tab for the instance
        -parameters:
            index: the index at which to place the new tab (-1 if at the end)
        */

        Editor* current();
        /* -access current tab object */

        Editor* operator[](const int index);
        /* -directly access instances (tabs) of editor */

    private slots:
        void close(int index);
        /* -close tab of index 'index' */
};

#endif // EDITORTABBAR_H
