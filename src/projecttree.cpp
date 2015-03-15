/*
Project: Lepton Editor
File: projecttree.cpp
Author: Leonardo Banderali
Created: March 14, 2015
Last Modified: March 15, 2015

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



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTree::ProjectTree(QObject* parent) : QAbstractItemModel(parent) {
}

ProjectTree::~ProjectTree() {
}



//~reimplemented virtual methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QModelIndex ProjectTree::index(int row, int column, const QModelIndex & parent) const {
    return QModelIndex();
}

QModelIndex ProjectTree::parent(const QModelIndex & index) const {
    return QModelIndex();
}

int ProjectTree::rowCount(const QModelIndex & parent) const {
    return 0;
}

int ProjectTree::columnCount(const QModelIndex & parent) const {
    return 0;
}

QVariant ProjectTree::data(const QModelIndex & index, int role) const {
    return QVariant();
}

Qt::ItemFlags ProjectTree::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;
    else
        return QAbstractItemModel::flags(index);
}

QVariant ProjectTree::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

