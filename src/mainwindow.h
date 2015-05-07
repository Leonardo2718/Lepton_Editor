/*
Project: Lepton Editor
File: mainwindow.h
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: May 7, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the definition of the main window object.

Copyright (C) 2015 Leonardo Banderali

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
#include <QTreeView>
#include <QPoint>
#include <QModelIndex>
#include <QString>

#include "editortabbar.h"
//#include "projecttree.h"
#include "projecttreemodel.h"
#include "settingseditor.h"
#include "findreplacedialog.h"

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
        void on_actionOpen_File_triggered();
        /* -open a file in an editor tab */

        void on_actionNew_File_triggered();
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
        /* -called when visible tab is changed to update main window */

        void on_actionOpen_Project_triggered();
        /* -called to add a new project directory to tree model */

        void on_actionNew_Project_triggered();
        /* -called to create and add a new project to tree model */

        void projectItemContextMenuRequested(const QPoint& position);
        /* -called when project item is right-clicked */

        void openFileRequested(const QString& filePath);
        /* -called when an object (ex. project list) requests to open a file */

        void on_actionGitHub_Page_triggered();
        /* -called to open the Lepton Editor GitHub page in the user's default web browser */

        void on_actionAbout_Lepton_Editor_triggered();
        /* -called to display an "about window" for Lepton */

        void on_actionAbout_Qt_triggered();
        /* -called to display an "about window" for Qt */

        void on_actionSettings_Editor_triggered();
        /*  -opens the settings editor window */

        void changeSpaceTabUse(QAction* actionTriggered);
        /*  -sets the current editor tab to use tabs or spaces, depending on `actionTriggered` */

        void on_actionReplace_Tabs_with_Spaces_triggered();
        /*  -called to replace tabs with spaces */

        void on_actionReplace_Spaces_with_Tabs_triggered();
        /*  -called to replace spaces with tabs */

        void on_action_Find_Replace_triggered();
        /*  -called to show the Find/Replace dialog */

        void findInCurrent(const FindReplaceDialog::DialogParameters& parameters);
        /*  -called to perform a "find" on the current tab file using the data in `parameters` */

        void findNextInCurrent();
        /*  -called to perform a "find next" on the current tab file (should be called after `findInCurrent()` ) */

        void replaceInCurrent(const FindReplaceDialog::DialogParameters& parameters);
        /*  -called to perform a "replace" on the current tab file using the data in `parameters` */

    private:
        Ui::MainWindow* ui;             //a pointer to the interface used to interact with the main window
        EditorTabBar* editors;          //pointer to editor tab bar object
        //ProjectModel* projectListModel; //model which will display project list
        ProjectTreeModel* projectTree;
        QTreeView* projectList;         //view in which projects will be displayed
        SettingsEditor configsEditor;   //a window to facilitate editing config files
        QActionGroup* selectorSpaceTab; //group of actions to select whether to use spaces or tabs
        FindReplaceDialog  findReplace; //a dialog used to do find/replace tasks

        void openFile(const QString& filePath);
        /* -opens a specified file in an editor tab */

        void saveFile(int index);
        /* -save content to open file */

        void saveFileAs(int index);
        /* -save content to a new file and load it */

        void saveFileCopyAs(int index);
        /* -save a copy of content to a new file (new file not loaded) */

        void setLanguageSelectorMenu();
        /* -set the language selector menu from editor object */

        void setSpaceTabSelector();
        /*  -set the space/tab selector to match the state of the current editor tab */

        void loadSession();
        /* -load settings and configs from saved session */

        void saveSession();
        /* -save settings and configs of this session */

        QString getDialogDirPath(const QString& location = 0);
        /*
            -gets path to a directory for dialogs to open
            -returns directory of file open in current editor tab when `location = 0` and when a file is actually open
            -returns home directory (or executable's directory if `QT_DEBUG` is defined) when no file is open or `location = "home"`
        */
};

#endif // MAINWINDOW_H
