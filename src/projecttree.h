/*
Project: Lepton Editor
File: projecttree.h
Author: Leonardo Banderali
Created: March 14, 2015
Last Modified: March 23, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTree class is a subclass of QAbstractItemModel that represents a list of
    Lepton projects in a tree structure.  It is used to display a list of projects in a
    QTreeView object.

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

#ifndef PROJECTTREE_H
#define PROJECTTREE_H

// include Qt classes
#include <QAbstractItemModel>
#include <QList>
#include <QAction>

// include other project headers
#include "leptonproject.h"

class ProjectTree : public QAbstractItemModel {
    Q_OBJECT

    public:
        // constructors and destructor
        explicit ProjectTree(QObject* parent = 0);
        ~ProjectTree();

        // reimplemented pure virtual methods (see QAbstractItemModel documentation)
        QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex & index) const;
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        int columnCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

        // reimplemented virtual methods (see QAbstractItemModel documentation)
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        // other public methods

        QList<QAction*> getActionsFor(const QModelIndex& index);
        /* -returns the context menu actions for a particular project item specified by `index` */

    private:
        QList<LeptonProject*> projects;
};

#endif // PROJECTTREE_H
