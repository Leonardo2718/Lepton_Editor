/*
Project: Lepton Editor
File: projectlistmodel.h
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 18, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectListModel is a subclass of QAbstractItemModel that represents a list of
    Lepton projects. It is used to display the list of projects in the project manager,
    which is an instance of QTreeView objec.

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

#ifndef PROJECTLISTMODEL_H
#define PROJECTLISTMODEL_H

// project headers
#include "projectlistitem.h"

// Qt classes
#include <QAbstractItemModel>

// c++ standard libraries
#include <memory>



/*
The ProjectListModel is a subclass of QAbstractItemModel that represents a list of Lepton projects.
The model stores information about a project as well as its contents. At the most basic level, a
project is just a directory in the file system. So, the project manager acts a lot like a file
browser. Because of this, it makes sense to both store and display projects as a tree structure.
Note however that QFileSystemModel cannot be used here because not all projects are guarentied to
be in the same directory.
*/
class ProjectListModel : public QAbstractItemModel {
    Q_OBJECT

    public:
        ProjectListModel();

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex{}) const noexcept;

        QModelIndex parent(const QModelIndex& index) const noexcept;

        int rowCount(const QModelIndex & parent = QModelIndex()) const noexcept;

        int columnCount(const QModelIndex & parent = QModelIndex()) const noexcept;

        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const noexcept;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const noexcept;

        bool addItem(const QModelIndex& parent = QModelIndex());

        void loadSession();
        /*  load projects saved from previous session */

        void saveSession();
        /*  save open projects from current session */

        QList<QAction*> contextActionsFor(const QModelIndex index);
        /*  returns the context menu actions for the item at `index` */

    signals:
        void requestOpenFile(const QFileInfo& fileInfo);
        /*  notifies that a file needs to be open */

    public slots:
        void itemDoubleClicked(const QModelIndex& index);
        /*  handle a view item double click */

        bool openProject();
        /*  opens an existing project */

    private:
        std::unique_ptr<ProjectListRoot> root;    // root of the model

        void loadAllChildrenOf(QList<ProjectListItem*> nodes);
        /*  loads the children (and grandchildren) of all `nodes` */

    private slots:
        void changeDataActionTriggered(bool);
        /*  called when a data changing action of an item is triggered */

        void newChildActionTriggered(bool);
        /*  called when an add child action of an item is triggered */

        void removeActionTriggered(bool);
        /*  called when a remove action of an item is triggered */
};



#endif // PROJECTLISTMODEL_H
