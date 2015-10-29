/*
Project: Lepton Editor
File: projectlistmodel.cpp
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 29, 2015

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
#include "leptonconfig.h"
#include "sessionmanager.h"

// Qt classes
#include <QFileDialog>



//~class implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectListModel::ProjectListModel() : root{std::make_unique<ProjectListRoot>()} {
}

QModelIndex ProjectListModel::index(int row, int column, const QModelIndex& parent) const noexcept {
    auto p = static_cast<ProjectListItem*>(parent.internalPointer());
    if (p != nullptr && column == 0 && row < p->childCount())
        return createIndex(row, column, static_cast<void*>(p->childAt(row)));
    else if (p == nullptr && column == 0)
        return createIndex(row, column,  static_cast<void*>(root->childAt(row)));
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
            row = grandParent->indexOfChild(parent);
        }
        return createIndex(row, 0, static_cast<void*>(parent));
    }
    else {
        return QModelIndex{};
    }
}

int ProjectListModel::rowCount(const QModelIndex & parent) const noexcept {
    auto p = static_cast<ProjectListItem*>(parent.internalPointer());
    if (p != nullptr)
        return p->childCount();
    else
        return root->childCount();
}

int ProjectListModel::columnCount(const QModelIndex & parent) const noexcept {
    return 1;
}

QVariant ProjectListModel::data(const QModelIndex & index, int role) const noexcept {
    auto item = static_cast<ProjectListItem*>(index.internalPointer());

    if (item != nullptr && item != root.get())
        return item->data(role);
    else
        return QVariant{};
}

QVariant ProjectListModel::headerData(int section, Qt::Orientation orientation, int role) const noexcept {
    if (orientation == Qt::Horizontal && section == 0) {
        return root->data(role);
    }
    else {
        return QVariant{};
    }
}

/*
load projects saved from previous session
*/
void ProjectListModel::loadSession() {
    SessionManager session;
    QVariantList projectList = session.value("projectPathList").toList();

    // get list of all projects that need to be opened
    QList<QString> projectPaths;
    foreach(const QVariant& projectEntry, projectList) {
        projectPaths.append(projectEntry.toMap().value("project_path").toString());
    }

    // remove old items
    beginRemoveRows(createIndex(0,0, nullptr), 0, projectList.size() - 1);
    for (int i = root->childCount() - 1; i >= 0; i--) {
        root->removeChild(i);
    }
    endRemoveRows();

    // create an load all the projects
    beginInsertRows(createIndex(0,0, nullptr), 0, projectList.size() - 1);
    ProjectListItem::ChildList projects = root->loadProjects(projectPaths);
    QList<ProjectListItem*> treeNodes;
    for (auto& node : projects) {
        treeNodes.append(node.get());
        root->addChild(std::move(node));
    }
    loadAllChildrenOf(treeNodes);
    endInsertRows();
}

/*
save open projects from current session
*/
void ProjectListModel::saveSession() {
    SessionManager session;
    QVariantList projectList;
    const int projectCount = root->childCount();

    for (int i = 0; i < projectCount; i++) {
        auto project = dynamic_cast<Project*>(root->childAt(i));
        if (project != nullptr) {
            QVariantMap d;
            d.insert("project_path", project->path());
            projectList.append(d);
        }
    }

    session.setValue("projectPathList", projectList);
}

/*
returns the context menu actions for the item at `index`
*/
QList<QAction*> ProjectListModel::contextActionsFor(const QModelIndex index) {
    auto item = static_cast<ProjectListItem*>(index.internalPointer());

    // get all the actions and convert them to `QActions*`
    QList<QAction*> actionList;
    foreach (ProjectItemAction* action, item->contextMenuActions()) {
        actionList.append(action);
    }
    return actionList;
}

/*
opens an existing project
*/
bool ProjectListModel::openProject() {
    auto projectPath = QFileDialog::getExistingDirectory(nullptr, "Open project");
    if (!projectPath.isEmpty()) {

        std::unique_ptr<ProjectListItem> item = root->loadProject(projectPath); // construct the tree node
        QList<ProjectListItem*> treeNodes;
        treeNodes.append(item.get());
        loadAllChildrenOf(treeNodes);

        // add the new node (and its children) to the tree
        beginInsertRows(QModelIndex(), root->childCount(), root->childCount());
        root->addChild(std::move(item));
        endInsertRows();

        return true;
    }
    else {
        return false;
    }
}

/*
handle a view item double click
*/
void ProjectListModel::itemDoubleClicked(const QModelIndex& index) {
    auto itemPtr = static_cast<ProjectListItem*>(index.internalPointer());
    auto fsItemPtr = dynamic_cast<ProjecFileSystemItem*>(itemPtr);
    if (fsItemPtr != nullptr) {
        auto itemPath = QFileInfo(fsItemPtr->path());
        if (itemPath.isFile()) {
            emit requestOpenFile(itemPath);
        }
    }
}

/*
loads the children (and grandchildren) of all `nodes`
*/
void ProjectListModel::loadAllChildrenOf(QList<ProjectListItem*> nodes) {
    for (int i = 0; i < nodes.size(); i++) {
        // connect change data actions
        foreach (ProjectItemAction* action, nodes.at(i)->changeDataActions()) {
            connect(action, SIGNAL(triggered(bool)), this, SLOT(changeDataActionTriggered(bool)));
        }

        // connect new child actions
        foreach (ProjectItemAction* action, nodes.at(i)->newChildActions()) {
            connect(action, SIGNAL(triggered(bool)), this, SLOT(newChildActionTriggered(bool)));
        }

        // connect remove actions
        foreach (ProjectItemAction* action, nodes.at(i)->removeActions()) {
            connect(action, SIGNAL(triggered(bool)), this, SLOT(removeActionTriggered(bool)));
        }

        // get all children of the node (to load them on the next iterations of the loop)
        ProjectListItem::ChildList newNodes = nodes.at(i)->loadChildren();
        for (auto& node : newNodes) {
            nodes.append(node.get());
            nodes.at(i)->addChild(std::move(node));
        }
    }
}

/*
called when a data changing action of an item is triggered
*/
void ProjectListModel::changeDataActionTriggered(bool) {
    ProjectItemAction* action = dynamic_cast<ProjectItemAction*>(sender());
    if (action != nullptr) {
        ProjectListItem* item = action->item();
        if (item != nullptr) {
            bool handled = item->handleChangeDataAction(action);    // let the item handle the action

            // if the action was handled
            if (handled) {
                // get the model index of the item
                int index = 0;
                ProjectListItem* parentItem = item->parent();
                if (parentItem != nullptr)
                    index = item->parent()->indexOfChild(item);
                else
                    index = root->indexOfChild(item);
                QModelIndex itemIndex = createIndex(index, 0, static_cast<void*>(item));

                // notify views of the change
                emit dataChanged(itemIndex, itemIndex);
            }
        }
    }
}

/*
called when an add child action of an item is triggered
*/
void ProjectListModel::newChildActionTriggered(bool) {
    ProjectItemAction* action = dynamic_cast<ProjectItemAction*>(sender());
    if (action != nullptr) {
        QModelIndex itemIndex;
        ProjectListItem* item = action->item();
        if (item != nullptr) {
            ProjectListItem::ChildPtr child = item->handleNewChildAction(action);   // let the item handle the action

            // if the action was handled
            if (child.get() != nullptr) {
                // load all children of the new item
                QList<ProjectListItem*> children;
                children.append(child.get());
                loadAllChildrenOf(children);

                // get the model index of the new item
                int index = 0;
                ProjectListItem* parentItem = item->parent();
                if (parentItem != nullptr)
                    index = item->parent()->indexOfChild(item);
                else
                    index = root->indexOfChild(item);
                itemIndex = createIndex(index, 0, static_cast<void*>(item));

                // notify views of the change and add the new item node to the tree
                beginInsertRows(itemIndex, item->childCount(), item->childCount());
                item->addChild(std::move(child));
                endInsertRows();
            }
        }
    }
}

/*
called when a remove action of an item is triggered
*/
void ProjectListModel::removeActionTriggered(bool) {
    ProjectItemAction* action = dynamic_cast<ProjectItemAction*>(sender());
    if (action != nullptr) {
        QModelIndex itemIndex;
        ProjectListItem* item = action->item();
        if (item != nullptr) {
            bool handled = item->handleRemoveAction(action);    // let the item handle the action

            // if the action was handled
            if (handled) {
                // get the model index of the item
                int index = 0;
                ProjectListItem* parentItem = item->parent();
                if (parentItem != nullptr)
                    index = item->parent()->indexOfChild(item);
                else
                    index = root->indexOfChild(item);
                itemIndex = createIndex(index, 0, static_cast<void*>(item));

                // notify views of the change and remove the item node from the tree
                beginRemoveRows(parent(itemIndex), index, index);
                parentItem->removeChild(index);
                endRemoveRows();
            }
        }
    }
}
