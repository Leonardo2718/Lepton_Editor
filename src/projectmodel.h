/*
Project: Lepton Editor
File: projectmodel.h
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: September 3, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectModel class is intended to be a data model class to represent a directory or group of files
    which together form a programming project.  This class is used to access and manipulate source files at
    the project level.  A class to define a dialog  box to create a new project is also defined here.

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

#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDialog>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QActionGroup>

#include <projectitem.h>

class ProjectCreatorDialog; //class for a project creator dialog box

class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT
    public:
        explicit ProjectModel(QObject *parent = 0);
        ~ProjectModel();

        bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
        /* -removes a row from the model */

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
        /*
        -returns the model index of an item specified by its 'row' and 'column' numbers and its parent item
        -if the item is not in the model, an empty index is returned
        */

        QVariant data(const QModelIndex& index, int role) const;
        /* -returns data associated with an item stored under a given 'role' */

        QModelIndex parent(const QModelIndex& child) const;
        /* -returns the parent item model index of a specified item */

        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        /* -returns data to be put in header of table (header in tree view) */

        QList< QAction* > getActionsFor(const QModelIndex& index);
        /* -returns all actions which can be performed on a given item */

        int rowCount(const QModelIndex& parent) const;
        /* -returns the number of rows in a model item */

        int columnCount(const QModelIndex& parent) const;
        /* -returns the number of columns in a model item */

        Qt::ItemFlags flags(const QModelIndex& index) const;
        /* -returns the flags which correspond to the item referenced by 'index' */

        bool setData(const QModelIndex &index, const QVariant &value, int role);
        /* -sets the data for an item and returns true if succesful */

        int projectCount();
        /*  -return the number of projects in the model */


    signals:
        void openFileRequested(const QString& filePath);
        /* emited when a project file is to be opened */

    public slots:
        void openProjectRequest();
        /* -called by user to open a project */

        void newProjectRequest();
        /* -called by user to create a new project */

    private slots:
        void actionOnFileTriggered(QAction* action);
        /* -called when action from project file context menu is triggered */

        void actionOnDirTriggered(QAction* action);
        /* -called when action from project directory context menu is triggered */

        void actionOnProjectTriggered(QAction* action);
        /* -called when action from project context menu is triggered */

    private:
        ProjectItem* rootProjectItem;   //item to hold root of project list
        QModelIndex actionEmiter;       //index of item on which to execute an action
        QActionGroup* fileActions;      //actions to be executed on a file
        QActionGroup* dirActions;       //actions to be executed on a directory
        QActionGroup* projectActions;   //actions to be executed on a project

        void addProject(const QString& projectPath);
        /* -builds tree for a new project and adds it to the list */

        void populateProject(ProjectItem* projectRoot);
        /* -populates a project item with its child items */
};

/*
-class for a project creator dialog box
*/
class ProjectCreatorDialog : public QDialog {
    Q_OBJECT

    public:
        explicit ProjectCreatorDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
        ~ProjectCreatorDialog();

        QString getProjectName();
        /* -returns entered project name */

        QString getProjectPath();
        /* -returns entered project path */

    public slots:
        void browseClicked();

    private slots:
        void accept();
        void reject();

    private:
        QString projectName;
        QString projectPath;

        QHBoxLayout* mainLayout;
        QGridLayout* creatorFormLayout;
        QLineEdit* projectNameField;
        QLineEdit* projectPathField;
        QPushButton* directoryBrowse;
        QList< QLabel* > labels;
        QDialogButtonBox* buttonBox;
};

#endif // PROJECTMODEL_H
