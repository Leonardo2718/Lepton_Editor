/*
Project: Lepton Editor
File: projecttreemodel.cpp
Author: Leonardo Banderali
Created: March 14, 2015
Last Modified: May 6, 2015

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
#include <QInputDialog>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTreeModel::ProjectTreeModel(QObject* parent) : QAbstractItemModel(parent) {
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
    emit layoutChanged();
}

/*
-handles an action triggered from an item's context menu
*/
void ProjectTreeModel::handleContextMenuAction(QAction* actionTriggered) {
    QString actionData = actionTriggered->data().toString();
    const bool isDir = lastItemSelected->getDataItem("is_directory").toBool();
    const bool isFile = lastItemSelected->getDataItem("is_file").toBool();

    if (isDir && actionData == "%ADD_FILE"){
        const QString path = lastItemSelected->getDataItem("path").toString();
        QString fileName = QFileDialog::getSaveFileName(0, "New File", path);
        if (!fileName.isEmpty()) {
            const ProjectTreeItem* p = lastItemSelected->getParent();

            /*#######################################################################################
            ### Here, it would make sense to call `beginInsertRows()` since we are adding an item. ##
            ### But, because the project item will be reloaded, some pointers are going to be      ##
            ### deleted and re-allocated.  So, `beginRemoveRows()` MUST be called to notify the    ##
            ### view that its internal pointers are no longer valid and avoid any potential        ##
            ### segfaults.  Also, we don't know ahead of time the index of the new row because it  ##
            ### gets created when the item is reloaded.                                            ##
            ###                                                                                    ##
            ### (This makes no semantic sense.  Why are we having to call `beginRemoveRows()` and  ##
            ### `endRemoveRows()` when we're actually ADDING a row?!  The project item NEEDS to be ##
            ### reloaded becuase a change in its internal structure can change the item itself.    ##
            ### Also, there is no way that I know of to really predict where the newly inserted    ##
            ### item will end up so some pointer deletion and re-allocation is bound to happen.    ##
            ### Whoever designed this library clearly didn't think of this use case.  All that's   ##
            ### really needed is just an additional function that simply informs the view that     ##
            ### it's internal pointers are no longer valid but DOES NOT IMPLY THAT AN ITEM MUST BE ##
            ### REMOVED.  Anyway, using `beginRemoveRows()` and `endRemoveRows()` will have to do  ##
            ### for now.  It solves the problem of the random segfaults.)                          ##
            #######################################################################################*/

            beginRemoveRows(indexFor(p), 0, p->childCount());
            QFile newFile(fileName);
            newFile.open(QFile::ReadWrite); // creates the file in case it does not yet exist
            newFile.close();
            lastItemSelected->reload();
            endRemoveRows();
        }
    } else if (isDir && actionData == "%ADD_DIRECTORY") {
        const QString path = lastItemSelected->getDataItem("path").toString();
        QString dirName = QFileDialog::getSaveFileName(0, "New Directory", path, QString(),0,QFileDialog::ShowDirsOnly);
        if (!dirName.isEmpty()) {
            const ProjectTreeItem* p = lastItemSelected->getParent();
            beginRemoveRows(indexFor(p), 0, p->childCount());
            QDir dir(path);
            dir.mkpath(dirName);
            lastItemSelected->reload();
            endRemoveRows();
        }
    } else if (isDir && actionData == "%RENAME_DIR") {
        /*const QString path = lastItemSelected->getDataItem("path").toString();
        QString newName = QInputDialog::getText(0, "Rename Directory", tr("Change directory name from \"%0\" to:").arg(lastItemSelected->getDataItem("name").toString()));
        if (!newName.isEmpty()) {
            ProjectTreeItem* p = (ProjectTreeItem*)lastItemSelected->getParent();
            beginRemoveRows(indexFor(p), 0, p->childCount());
            QDir dir(path);
            dir.cdUp();
            dir.rename(lastItemSelected->getDataItem("name").toString(), newName);
            disconnect(lastItemSelected->getContextMenuActions(), SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));
                                    // item signals should be disconnected as it will be deleted.
            lastItemSelected = 0;   // since the item will be deleted, it should no be pointed to anymore
            p->reload();            // the parent must be reloaded since the item was removed
            endRemoveRows();
        }*/
    } else if (isDir && actionData == "%REMOVE_DIR") {
        ProjectTreeItem* p = (ProjectTreeItem*)lastItemSelected->getParent();
        beginRemoveRows(indexFor(p), 0, p->childCount());
        QDir dir(lastItemSelected->getDataItem("path").toString());
        dir.removeRecursively();
        disconnect(lastItemSelected->getContextMenuActions(), SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));
                                // item signals should be disconnected as it will be deleted.
        lastItemSelected = 0;   // since the item will be deleted, it should no be pointed to anymore
        p->reload();            // the parent must be reloaded since the item was removed
        endRemoveRows();
    } else if (isFile && actionData == "%OPEN_FILE") {
    } else if (isFile && actionData == "%RENAME_FILE") {
        /*QString newName = QInputDialog::getText(0, "Rename File", tr("Change file name from \"%0\" to:").arg(data.value("name").toString()));
        if (!newName.isEmpty()) {
            QDir dir = QFileInfo(path).absoluteDir();
            dir.rename(data.value("name").toString(), newName);
            reloadProject();
        }*/
    } else if (isFile && actionData == "%DELETE_FILE") {
        ProjectTreeItem* p = (ProjectTreeItem*)lastItemSelected->getParent();
        beginRemoveRows(indexFor(p), 0, p->childCount());
        QDir dir(p->getDataItem("path").toString());
        dir.remove(lastItemSelected->getDataItem("path").toString());
        disconnect(lastItemSelected->getContextMenuActions(), SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));
                                // item signals should be disconnected as it will be deleted.
        lastItemSelected = 0;   // since the item will be deleted, it should no be pointed to anymore
        p->reload();            // the parent must be reloaded since the item was removed
        endRemoveRows();
    } else if (actionData == "%REFRESH") {
        const ProjectTreeItem* p = lastItemSelected->getParent();
        beginRemoveRows(indexFor(p), 0, p->childCount());
        lastItemSelected->reload();
        endRemoveRows();
    } else {
    }
}



//~private functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-returns the model index for a given model item
*/
QModelIndex ProjectTreeModel::indexFor(const ProjectTreeItem* item) {
    if (item != 0 && item != projects) {
        const ProjectTreeItem* p = item->getParent();
        int col = 0;
        int row = p->getChildIndex(item);
        return createIndex(row, col, (void*)item);
    } else {
        return createIndex(0, 0, (void*)0);
    }
}

