/*
Project: Lepton Editor
File: projectlistmodel.cpp
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

// project headers
#include "projectlistmodel.h"
#include "leptonconfig.h"

// Qt classes
#include <QSettings>
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
    //set session object information
    //QSettings::setDefaultFormat(QSettings::NativeFormat);   //%%% I may decide to change this later on and use my own format
    //QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, LeptonConfig::mainSettings->getConfigDirPath("sessions"));

    QSettings session;
    //beginInsertRows(createIndex(0,0, root.get()), 0, root->childCount());
    QVariantList projectList = session.value("projectPathList").toList();
    beginInsertRows(createIndex(0,0, nullptr), 0, projectList.size() - 1);
    /*foreach (const QVariant& projectEntry, projectList) {
        QVariantMap d = projectEntry.toMap();
        QVariantList commands{};
        commands.append(QString("load"));
        commands.append(d.value("project_path"));
        root->addChild(commands);
    }*/
    QList<QString> projectPaths;
    foreach(const QVariant& projectEntry, projectList) {
        projectPaths.append(projectEntry.toMap().value("project_path").toString());
    }
    QList<ProjectListItem*> treeNodes = root->loadProjects(projectPaths);
    for (int i = 0; !treeNodes.isEmpty() && i < treeNodes.size(); i++) {
        foreach (ProjectItemAction* action, treeNodes.at(i)->removeActions()) {
            connect(action, SIGNAL(triggered(bool)), this, SLOT(removeActionTriggered(bool)));
        }
        QList<ProjectListItem*> newNodes = treeNodes.at(i)->loadChildren();
        treeNodes.append(newNodes);
    }
    endInsertRows();
}

/*
save open projects from current session
*/
void ProjectListModel::saveSession() {
    //set session object information
    //QSettings::setDefaultFormat(QSettings::NativeFormat);   //%%% I may decide to change this later on and use my own format
    //QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, LeptonConfig::mainSettings->getConfigDirPath("sessions"));

    QSettings session;
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
    QList<QAction*> actionList;
    foreach (ProjectItemAction* action, item->contextMenuActions()) {
        actionList.append(static_cast<QAction*>(action));
    }
    return actionList;
}

/*
opens an existing project
*/
bool ProjectListModel::openProject() {
    auto projectPath = QFileDialog::getExistingDirectory(nullptr, "Open project");
    if (!projectPath.isEmpty()) {
        beginInsertRows(QModelIndex(), root->childCount(), root->childCount());
        ProjectListItem* item = root->loadProject(projectPath);
        foreach (ProjectItemAction* action, item->removeActions()) {
            connect(action, SIGNAL(triggered(bool)), this, SLOT(removeActionTriggered(bool)));
        }
        QList<ProjectListItem*> treeNodes = item->loadChildren();
        for (int i = 0; !treeNodes.isEmpty() && i < treeNodes.size(); i++) {
            QList<ProjectListItem*> newNodes = treeNodes.at(i)->loadChildren();
            treeNodes.append(newNodes);
        }
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
called when a remove action of an item is triggered
*/
void ProjectListModel::removeActionTriggered(bool) {
    ProjectItemAction* action = dynamic_cast<ProjectItemAction*>(sender());
    if (action != nullptr) {
        QModelIndex itemIndex;// = createIndex()
        ProjectListItem* item = action->item();
        int index = 0;
        if (item != nullptr && item->parent() != nullptr)
            index = item->parent()->indexOfChild(item);
        else if (item != nullptr && item->parent() == nullptr)
            index = root->indexOfChild(item);
        itemIndex = createIndex(index, 0, static_cast<void*>(item));
        beginRemoveRows(parent(itemIndex), index, index);
        item->handleRemoveAction(action);
        endRemoveRows();
    }
}
