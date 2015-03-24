/*
Project: Lepton Editor
File: projecttree.cpp
Author: Leonardo Banderali
Created: March 14, 2015
Last Modified: March 20, 2015

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

#include "projecttree.h"

// include Qt classes
#include <QFileIconProvider>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTree::ProjectTree(QObject* parent) : QAbstractItemModel(parent) {
    projects.append(new LeptonProject("test_project"));
}

ProjectTree::~ProjectTree() {
}



//~reimplemented virtual methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QModelIndex ProjectTree::index(int row, int column, const QModelIndex & parent) const {
    QModelIndex m;
    const LeptonProjectItem* p = static_cast<const LeptonProjectItem*>(parent.internalPointer());
    if (column != 0 || row < 0)
        return m;
    else if (p == 0) {
        if (row < projects.count())
            m = createIndex(row, column, projects[row]);
    } else if (p->hasChildren()) {
        if (row < p->childCount())
            m = createIndex(row, column, (void*)p->getChild(row));
    }
    return m;
}

QModelIndex ProjectTree::parent(const QModelIndex & index) const {
    QModelIndex m;
    const LeptonProjectItem* item = static_cast<const LeptonProjectItem*>(index.internalPointer());

    if (item == 0)  // if the current item is null (its the root) it can't have a parent
        return m;

    const LeptonProjectItem* itemParent = dynamic_cast<const LeptonProjectItem*>(item->getParent());

    if (itemParent == 0)
        // if the parent is null then the parent is the root
        m = createIndex(0, 0, (void*)0);
    else if (itemParent->getParent() == 0)
        // if the parents parent (current items grand parent) is null then the parent must be a project (stored in the list)
        m = createIndex(projects.indexOf((LeptonProject*)itemParent), 0, (void*)itemParent);
    else {
        // if both the parent and grand parent of the current item are not null then just use the position of the parent in
        // inside the grand parent
        const LeptonProjectItem* grandParent = static_cast<const LeptonProjectItem*>(itemParent->getParent());
        m = createIndex(grandParent->getChildIndex((LeptonProjectItem*)itemParent), 0 , (void*)itemParent);
    }
    return m;
}

int ProjectTree::rowCount(const QModelIndex & parent) const {
    if (parent.internalPointer() == 0)
        return projects.count();
    else
        return static_cast<const LeptonProjectItem*>(parent.internalPointer())->childCount();
}

int ProjectTree::columnCount(const QModelIndex & parent) const {
    return 1;
}

QVariant ProjectTree::data(const QModelIndex & index, int role) const {
    QVariant r;
    if (index.internalPointer() == 0 || index.column() != 0)
        return QVariant();
    else {
        const LeptonProjectItem* item = static_cast<const LeptonProjectItem*>(index.internalPointer());
        if (role == Qt::DisplayRole)
            r = QVariant(item->getName());
        else if (role == Qt::DecorationRole) {
            QFileIconProvider iconProvider;
            if (item->hasChildren())
                r = QVariant(iconProvider.icon(QFileIconProvider::Folder));
            else
                r = QVariant(iconProvider.icon(QFileIconProvider::File));
        }
    }
    return r;
}

Qt::ItemFlags ProjectTree::flags(const QModelIndex &index) const {
    if (!index.isValid() || index.column() != 0)
        return Qt::NoItemFlags;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ProjectTree::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
        return QVariant("Projects");
    else
        return QVariant();
}

