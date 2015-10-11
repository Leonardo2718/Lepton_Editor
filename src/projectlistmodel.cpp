/*
Project: Lepton Editor
File: projectlistmodel.cpp
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 10, 2015

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

// project headers
#include "projectlistmodel.h"



//~class implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectListModel::ProjectListModel() {
    //root = std::make_unique<ProjectListItem>(QFileInfo{});
}

QModelIndex ProjectListModel::index(int row, int column, const QModelIndex& parent) const noexcept {
    auto p = static_cast<ProjectListItem*>(parent.internalPointer());
    if (p != nullptr && column == 0 && row < p->childCount())
        return createIndex(row, column, static_cast<void*>(p->childAt(row)));
    else
        return QModelIndex{};
}

QModelIndex ProjectListModel::parent(const QModelIndex& index) const noexcept {
    auto item = static_cast<ProjectListItem*>(index.internalPointer());
    if (item != nullptr && item->parent() != nullptr) {
        auto parent = item->parent();
        int row = 0;
        if (parent != root.get()) {
            auto grandParent = parent->parent();
            /*auto ptr = std::unique_ptr<ProjectListItem>(parent);  // cast to unique_ptr to make comparison
            row = grandParent->children.indexOf(ptr);
            ptr.release();*/                                  // release unique_ptr to avoid deleting it
            row = grandParent->indexOfChild(parent);
        }
        return createIndex(row, 0, static_cast<void*>(parent));
    } else
        return QModelIndex{};
}

int ProjectListModel::rowCount(const QModelIndex & parent) const noexcept {
    auto p = static_cast<ProjectListItem*>(parent.internalPointer());
    return p->childCount();
}

int ProjectListModel::columnCount(const QModelIndex & parent) const noexcept {
    return 1;
}

QVariant ProjectListModel::data(const QModelIndex & index, int role) const noexcept {
    auto item = static_cast<ProjectListItem*>(index.internalPointer());

    if (item == nullptr)
        return item->data(role);
    else
        return QVariant{};
}
