/*
Project: Lepton Editor
File: projecttreemodel.cpp
Author: Leonardo Banderali
Created: March 14, 2015
Last Modified: April 5, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTreeModel class is a subclass of QAbstractItemModel that represents a list of
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

#include "projecttreemodel.h"

// include Qt classes



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTreeModel::ProjectTreeModel(QObject* parent) : QAbstractItemModel(parent) {
    //projects.append(new LeptonProject(this, "test_project"));
    //QVariantMap d;
    //d.insert("header_0", "Projects");
    projects = new ProjectTree();
    connect(projects, SIGNAL(changingItem(const ProjectTreeItem*)), this, SLOT(beginChangeItem(const ProjectTreeItem*)));
    connect(projects, SIGNAL(itemChanged()), this, SLOT(endChangeItem()));
    connect(projects, SIGNAL(removingItem(const ProjectTreeItem*)), this, SLOT(beginRemoveItem(const ProjectTreeItem*)));
    connect(projects, SIGNAL(itemRemoved()), this, SLOT(endRemoveItem()));
}

ProjectTreeModel::~ProjectTreeModel() {
    delete projects;
}



//~reimplemented virtual methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex & parent) const {
    QModelIndex m;
    //const LeptonProjectItem* p = static_cast<const LeptonProjectItem*>(parent.internalPointer());
    if (column != 0 || row < 0)
        return m;
    else if (!parent.isValid()) {
        if (row < projects->childCount())
            m = createIndex(row, column, (void*)projects->getChild(row));
    } else {
        const ProjectTreeItem* item = static_cast<const ProjectTreeItem*>(parent.internalPointer());
        if (row < item->childCount())
            m = createIndex(row, column, (void*)item->getChild(row));
    }
    return m;
}

QModelIndex ProjectTreeModel::parent(const QModelIndex & index) const {
    QModelIndex m;
    if (index.isValid()) {
        const ProjectTreeItem* p = (static_cast<const ProjectTreeItem*>(index.internalPointer()))->getParent();
        if (p == 0)
            // if the parent is null then the parent is the root
            m = createIndex(0, 0, (void*)0);
        else {
            const ProjectTreeItem* grandParent = static_cast<const ProjectTreeItem*>(p->getParent());
            if (grandParent == 0)
                m = createIndex(projects->getChildIndex(p), 0, (void*)p);
            else
                m = createIndex(grandParent->getChildIndex(p), 0 , (void*)p);
        }
    }
    return m;
}

int ProjectTreeModel::rowCount(const QModelIndex & parent) const {
    if (parent.internalPointer() == 0)
        return projects->childCount();
    else {
        const ProjectTreeItem* p = static_cast<const ProjectTreeItem*>(parent.internalPointer());
        p->hasChildren();
        return p->childCount();
    }
}

int ProjectTreeModel::columnCount(const QModelIndex & parent) const {
    return 1;
}

QVariant ProjectTreeModel::data(const QModelIndex & index, int role) const {
    QVariant r;
    if (index.internalPointer() == 0 || index.column() != 0)
        return QVariant();
    else {
        const ProjectTreeItem* item = static_cast<const ProjectTreeItem*>(index.internalPointer());
        if (role == Qt::DisplayRole)
            r = item->getDataItem("name");
        else if (role == Qt::DecorationRole) {
            r = item->getDataItem("icon");
        }
    }
    return r;
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid() || index.column() != 0)
        return Qt::NoItemFlags;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
        return QVariant("Projects");
    else
        return QVariant();
}



//~other public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-returns the context menu actions for a particular project item specified by `index`
*/
QList<QAction*> ProjectTreeModel::getActionsFor(const QModelIndex& index) {
    ProjectTreeItem* item = static_cast<ProjectTreeItem*>(index.internalPointer());
    return item->getContextMenuActions();
}



//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectTreeModel::newProjectRequest() {
    projects->createNewProject();
}

void ProjectTreeModel::openProjectRequest() {
    projects->openProject();
}



//~private slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectTreeModel::beginRemoveItem(const ProjectTreeItem* item) {
    const ProjectTreeItem* itemParent = item->getParent();
    int itemRow = itemParent->getChildIndex(item);
    const ProjectTreeItem* grandParent = itemParent->getParent();
    QModelIndex parentIndex;
    if (grandParent == 0)
        parentIndex = createIndex(0, 0, (void*)0);
    else
        parentIndex = createIndex(grandParent->getChildIndex(itemParent), 0, (void*)itemParent);
    this->beginRemoveRows(parentIndex, itemRow, itemRow);
}

void ProjectTreeModel::endRemoveItem() {
    this->endRemoveRows();
}

void ProjectTreeModel::beginChangeItem(const ProjectTreeItem* item) {
    emit layoutAboutToBeChanged();
}

void ProjectTreeModel::endChangeItem() {
    emit layoutChanged();
}

