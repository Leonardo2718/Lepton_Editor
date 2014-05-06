/*
Project: Lepton Editor
File: mainwindow.h
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: April 15, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the definition of the main window object.

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

#ifndef MAINWINDOW_H    //prevent the file from being included more than once
#define MAINWINDOW_H

//include all necessary files and libraries
#include <QMainWindow>
#include <QActionGroup>
#include "editor.h"
#include "editortabbar.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        /* -class constructor builds the main window */

        ~MainWindow();

    protected:
        void closeEvent(QCloseEvent *event);
        /* -called whenever a close window is requested */

    private slots:
        void on_actionOpen_triggered();
        /* -open a file in an editor tab */

        void on_actionNew_triggered();
        /* -open a new tab */

        void on_actionSave_triggered();
        /* -save content of tab to corresponding file */

        void save_signal_received(int index);
        /* -save contents of tab with specified index */

        void on_actionSave_As_triggered();
        /* -save content of tab to a new file */

        void on_actionSave_Copy_As_triggered();
        /* -save a copy of the content in the tab to a new file */

        void on_actionProject_Manager_toggled(bool visible);
        /* -show/hide the project manager */

        void on_actionEditor_Tools_toggled(bool visible);
        /* -show/hide the editor tools */

        void on_actionSave_All_triggered();
        /* -save changes to all documents */

        void editTabChanged();
        /* -called when visible tab is changed to update lanugage selector menu */

    private:
        Ui::MainWindow* ui;     //a pointer to the interface used to interact with the main window
        EditorTabBar* editors;
        QAction* langSelector;  //temporary pointer to language selector menu


        void saveFile(int index);
        /* -save content to open file */

        void saveFileAs(int index);
        /* -save content to a new file and load it */

        void saveFileCopyAs(int index);
        /* -save a copy of content to a new file (new file not loaded) */

        void setLanguageSelectorMenu();
        /* -set the language selector menu from editor object */
};

#endif // MAINWINDOW_H
