/*
Project: Lepton Editor
File: projectmodel.h
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: June 11, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectModel class is intended to be a data model class to represent a directory or group of files
    which together form a programming project.  This class is used to access and manipulate source files at
    the project level.

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
#include <projectitem.h>

class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT
    public:
        explicit ProjectModel(QObject *parent = 0);
        ~ProjectModel();

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

        int rowCount(const QModelIndex& parent) const;
        /* -returns the number of rows in a model item */

        int columnCount(const QModelIndex& parent) const;
        /* -returns the number of columns in a model item */

        //Qt::ItemFlag flags(const QModelIndex& index) const;
        /* -returns the flags which correspond to the item referenced by 'index' */

    signals:

    public slots:

    private:
        ProjectItem* rootProjectItem;   //item to hold root of project list

        void addProject(const QString& projectPath);
        /* -builds tree for a new project and adds it to the list */
};

#endif // PROJECTMODEL_H
