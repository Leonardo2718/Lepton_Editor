/*
Project: Lepton Editor
File: projecttreemodel.cpp
Author: Leonardo Banderali
Created: March 14, 2015
Last Modified: May 4, 2015

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
#include <QFileDialog>
#include <QDebug>


//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTreeModel::ProjectTreeModel(QObject* parent) : QAbstractItemModel(parent) {
    //projects.append(new LeptonProject(this, "test_project"));
    //QVariantMap d;
    //d.insert("header_0", "Projects");
    lastItemSelected = 0;
    projects = new ProjectTreeRoot();
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
    else if (!parent.isValid() || parent.internalPointer() == 0) {
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
    if (index.isValid() && index.internalPointer() != 0) {
        void* q = index.internalPointer();
        //qDebug() << q;
        const ProjectTreeItem* i = (const ProjectTreeItem*)q;
        //qDebug() << i->childCount();
        const ProjectTreeItem* p = i->getParent();
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
    if (!parent.isValid() || parent.internalPointer() == 0)
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
    if (lastItemSelected != 0)
        disconnect(lastItemSelected->getContextMenuActions(), SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));
    lastItemSelected = item;
    connect(lastItemSelected->getContextMenuActions(), SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));
    return lastItemSelected->getContextMenuActions()->actions();
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
    /*const ProjectTreeItem* itemParent = item->getParent();
    int itemRow = itemParent->getChildIndex(item);
    const ProjectTreeItem* grandParent = itemParent->getParent();
    QModelIndex parentIndex;
    if (grandParent == 0)
        parentIndex = createIndex(0, 0, (void*)0);
    else
        parentIndex = createIndex(grandParent->getChildIndex(itemParent), 0, (void*)itemParent);
    this->beginRemoveRows(parentIndex, itemRow, itemRow);*/
    if (projects->childCount() > 0)
        this->beginRemoveRows(index(0,0,QModelIndex()), 0, projects->childCount());
}

void ProjectTreeModel::endRemoveItem() {
    this->endRemoveRows();
}

void ProjectTreeModel::beginChangeItem(const ProjectTreeItem* item) {
    emit layoutAboutToBeChanged();
}

void ProjectTreeModel::endChangeItem() {
    /*const ProjectTreeItem* itemParent = item->getParent();
    int itemRow = itemParent->getChildIndex(item);
    const ProjectTreeItem* grandParent = itemParent->getParent();
    QModelIndex parentIndex;
    if (grandParent == 0)
        parentIndex = createIndex(0, 0, (void*)0);
    else
        parentIndex = createIndex(grandParent->getChildIndex(itemParent), 0, (void*)itemParent);*/
    //emit dataChanged(createIndex(0, 0, (void*)0), createIndex(0, 0, (void*)0));
    emit layoutChanged();
}

/*
-handles an action triggered from an item's context menu
*/
void ProjectTreeModel::handleContextMenuAction(QAction* actionTriggered) {
    QString actionData = actionTriggered->data().toString();
    qDebug() << actionData;
    const bool isDir = lastItemSelected->getDataItem("is_directory").toBool();
    const bool isFile = lastItemSelected->getDataItem("is_file").toBool();

    if (isDir && actionData == "%ADD_FILE"){
        const QString path = lastItemSelected->getDataItem("path").toString();
        QString fileName = QFileDialog::getSaveFileName(0, "New File", path);
        if (!fileName.isEmpty()) {
            QFile newFile(fileName);
            newFile.open(QFile::ReadWrite);     // creates the file if it does not yet exist
            newFile.close();
            lastItemSelected->reloadProject();
        }
    } else if (isDir && actionData == "%ADD_DIRECTORY") {
        /*QString dirName = QFileDialog::getSaveFileName(0, "New Directory", path, QString(),0,QFileDialog::ShowDirsOnly);
        if (!dirName.isEmpty()) {
            QDir dir(path);
            dir.mkpath(dirName);
            reloadProject();
        }*/
    } else if (isDir && actionData == "%RENAME_DIR") {
        /*QString newName = QInputDialog::getText(0, "Rename Directory", tr("Change directory name from \"%0\" to:").arg(data.value("name").toString()));
        if (!newName.isEmpty()) {
            QDir dir(path);
            dir.cdUp();
            dir.rename(data.value("name").toString(), newName);
            reloadProject();
        }*/
    } else if (isDir && actionData == "%REMOVE_DIR") {
    } else if (isFile && actionData == "%OPEN_FILE") {
    } else if (isFile && actionData == "%RENAME_FILE") {
        /*QString newName = QInputDialog::getText(0, "Rename File", tr("Change file name from \"%0\" to:").arg(data.value("name").toString()));
        if (!newName.isEmpty()) {
            QDir dir = QFileInfo(path).absoluteDir();
            dir.rename(data.value("name").toString(), newName);
            reloadProject();
        }*/
    } else if (isFile && actionData == "%DELETE_FILE") {
    } else if (actionData == "%REFRESH_PROJECT") {
        //reloadProject();
    } else {
    }
}

