/*
Project: Lepton Editor
File: projectlistitem.cpp
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 19, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectListItem class represents an item in the project list. Specializations
    of this class are used to implement different behavior for different types of items.

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
#include "projectlistitem.h"

// Qt classes
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QErrorMessage>
#include <QFile>
#include <QFileIconProvider>

#include <QDebug>

//~ProjectListItem implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectListItem::ProjectListItem() : QObject{0} {}

ProjectListItem::~ProjectListItem() noexcept = default;

ProjectListItem* ProjectListItem::parent() noexcept {
    return parentPtr;
}

int ProjectListItem::childCount() const noexcept {
    return children.size();
}

ProjectListItem* ProjectListItem::childAt(int row) noexcept {
    return children[row].get();
}

int ProjectListItem::indexOfChild(ProjectListItem* child) const noexcept {

    // perform linear search to find the index of the child (see note bellow)
    for (int i = 0, count = children.size(); i < count; i++) {
        if (children[i].get() == child) {
            return i;
        }
    }

    /*#############################################################################################
    ### Although it is usually considered bad practice to reimplement such a common algorithm    ##
    ### (here, the `QList::indexOf()` function does what we want), I did so in this case because ##
    ### `QList<unique_ptr>::indexOf()` requires the item being searched for to be a `unique_ptr`.##
    ### In order to meet this requirement, `child` would have to be wrapped into a temporary     ##
    ### `unique_ptr`. This could potentially cause a double delete if the temporary `unique_ptr` ##
    ### is not explicitly released.                                                              ##
    #############################################################################################*/

    return -1;
}

/*
adds an existing node to the tree
*/
void ProjectListItem::addChild(ChildPtr newChild) {
    newChild->parentPtr = this;
    children.push_back(std::move(newChild));
}

/*
removes a node from this item and returns it
*/
ProjectListItem::ChildPtr ProjectListItem::removeChild(int index) {
    auto childItr = children.cbegin() + index;
    auto oldChild = std::move(children[index]);
    children.erase(childItr);
    return oldChild;
}

/*
Returns the actions for the context menu to be displayed when this item is right-clicked in the
project manager. Any action within this group must store as its data a pointer to the item it
belongs to. By default this returns all the actions from `changeDataActions()`, `newChildActions()`,
and `removeActions()`.
*/
QList<ProjectItemAction*> ProjectListItem::contextMenuActions() const {
    QList<ProjectItemAction*> menuActions;
    menuActions.append(changeDataActions());
    menuActions.append(newChildActions());
    menuActions.append(removeActions());
    return menuActions;
}


/*
list of all actions that, when triggered, will cause a new child node to be created
*/
QList<ProjectItemAction*> ProjectListItem::newChildActions() const {
    return QList<ProjectItemAction*>{};
}

/*
list of all actions that, when triggered, will cause the node to be removed
*/
QList<ProjectItemAction*> ProjectListItem::removeActions() const {
    return QList<ProjectItemAction*>{};
}

/*
list of all actions that, when triggered, will cause the data of the node to be changed
*/
QList<ProjectItemAction*> ProjectListItem::changeDataActions() const {
    return QList<ProjectItemAction*>{};
}

/*
handles the creation of a new child
*/
ProjectListItem::ChildPtr ProjectListItem::handleNewChildAction(ProjectItemAction* newChildAction) {
    return false;
}

/*
handles the removal of this item
*/
bool ProjectListItem::handleRemoveAction(ProjectItemAction* removeAction) {
    return false;
}

/*
handles changing the data of this item
*/
bool ProjectListItem::handleChangeDataAction(ProjectItemAction* changeDataAction) {
    return false;
}



//~ProjectItemAction implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectItemAction::ProjectItemAction(const QString& _text, ProjectListItem* _item) : QAction{_text, 0}, projectItem{_item} {}

ProjectListItem* ProjectItemAction::item() const noexcept {
    return projectItem;
}



//~ProjectFile implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectFile::ProjectFile(const QFileInfo& _file) : file{_file} {
    renameAction = new ProjectItemAction{"Rename", this};
    deleteAction = new ProjectItemAction{"Delete", this};
}

ProjectFile::~ProjectFile() {
    delete renameAction;
    delete deleteAction;
}

QVariant ProjectFile::data(int role) const {
    if (role == Qt::DisplayRole)
        return QVariant{file.fileName()};
    else if (role == Qt::DecorationRole)
        return QVariant{QFileIconProvider{}.icon(QFileIconProvider::File)};
    else
        return QVariant{};
}

QString ProjectFile::path() const noexcept {
    return file.absoluteFilePath();
}

ProjectListItem::ChildList ProjectFile::loadChildren() {
    return ChildList{};
}

QList<ProjectItemAction*> ProjectFile::removeActions() const {
    QList<ProjectItemAction*> actions;
    actions.append(deleteAction);
    return actions;
}

QList<ProjectItemAction*> ProjectFile::changeDataActions() const {
    QList<ProjectItemAction*> actions;
    actions.append(renameAction);
    return actions;
}

bool ProjectFile::handleRemoveAction(ProjectItemAction* action) {
    bool actionHandled = false;

    if (action == deleteAction) {
        auto buttonPressed = QMessageBox::question(0, "Deleting File", QString("Are you sure you want to delete the file `%0`?").arg(data().toString()));
        if (buttonPressed == QMessageBox::Yes) {
            actionHandled = QFile::remove(file.absoluteFilePath());
        }
    }

    return actionHandled;
}

bool ProjectFile::handleChangeDataAction(ProjectItemAction* action) {
    bool actionHandled = false;

    if (action == renameAction) {
        QString newName = QInputDialog::getText(0, "Rename File", "New name: ", QLineEdit::Normal, file.fileName());
        if (!newName.isEmpty()) {
            QString oldFile = file.absoluteFilePath();
            file.setFile(file.absolutePath().append("/").append(newName));
            actionHandled = QFile::rename(oldFile, file.absoluteFilePath());
        }
    }

    return actionHandled;
}



//~ProjecDirectory implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectDirectory::ProjectDirectory(const QDir& _dir) : dir{_dir} {
    renameAction = new ProjectItemAction{"Rename", this};
    newFileAction = new ProjectItemAction{"Add file", this};
    newDirectoryAction = new ProjectItemAction{"Add directory", this};
    deleteAction = new ProjectItemAction{"Delete", this};
}

ProjectDirectory::~ProjectDirectory() {
    delete deleteAction;
    delete newDirectoryAction;
    delete newFileAction;
    delete renameAction;
}

QVariant ProjectDirectory::data(int role) const {
    if (role == Qt::DisplayRole)
        return QVariant{dir.dirName()};
    else if (role == Qt::DecorationRole)
        return QVariant{QFileIconProvider{}.icon(QFileIconProvider::Folder)};
    else
        return QVariant{};
}

QString ProjectDirectory::path() const noexcept {
    return dir.absolutePath();
}

ProjectListItem::ChildList ProjectDirectory::loadChildren() {
    ChildList children;

    // create a child item for each entry in the directory
    foreach (QFileInfo itemInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        if (itemInfo.isDir()) {
            children.push_back(std::make_unique<ProjectDirectory>(QDir(itemInfo.absoluteFilePath())));
        }
        else if (itemInfo.isFile()) {
            children.push_back(std::make_unique<ProjectFile>(itemInfo));
        }
    }

    return children;
}

QList<ProjectItemAction*> ProjectDirectory::newChildActions() const {
    QList<ProjectItemAction*> actions;
    actions.append(newFileAction);
    actions.append(newDirectoryAction);
    return actions;
}

QList<ProjectItemAction*> ProjectDirectory::removeActions() const {
    QList<ProjectItemAction*> actions;
    actions.append(deleteAction);
    return actions;
}

QList<ProjectItemAction*> ProjectDirectory::changeDataActions() const {
    QList<ProjectItemAction*> actions;
    actions.append(renameAction);
    return actions;
}

ProjectListItem::ChildPtr ProjectDirectory::handleNewChildAction(ProjectItemAction* action) {
    ChildPtr child = ChildPtr{nullptr};

    if (action == newFileAction) {
        QString name = QInputDialog::getText(0, "New File", "File name: ");
        if (!name.isEmpty()) {
            QFileInfo file{dir.absoluteFilePath(name)};
            if (!file.exists()) {
                QFile f{file.absoluteFilePath()};
                f.open(QIODevice::ReadWrite);
                f.close();
            }
            child = std::make_unique<ProjectFile>(file);
        }
    }
    else if (action == newDirectoryAction) {
        auto name = QInputDialog::getText(0, "New Directory", "Directory name: ");
        if (!name.isEmpty()) {
            dir.mkdir(name);
            child = std::make_unique<ProjectDirectory>(QDir{dir.absoluteFilePath(name)});
        }
    }

    return child;
}

bool ProjectDirectory::handleRemoveAction(ProjectItemAction* action) {
    bool actionHandled = false;

    if (action == deleteAction) {
        auto buttonPressed = QMessageBox::question(0, "Deleting Directory", QString("Are you sure you want to delete the directory `%0`?").arg(data().toString()));
        if (buttonPressed == QMessageBox::Yes) {
            actionHandled = dir.removeRecursively();
        }
    }

    return actionHandled;
}

bool ProjectDirectory::handleChangeDataAction(ProjectItemAction* action) {
    bool actionHandled = false;

    if (action == renameAction) {
        QString newName = QInputDialog::getText(0, "Rename Directory", "New name: ", QLineEdit::Normal, dir.dirName());
        if (!newName.isEmpty()) {
            QString oldName = dir.dirName();
            dir.cdUp();
            actionHandled = dir.rename(oldName, newName);
            if (actionHandled)
                dir.cd(newName);
            else
                dir.cd(oldName);
        }
    }

    return actionHandled;
}



//~Project implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Project::Project(const QDir& _projectDir) : ProjectDirectory{_projectDir}, projectDir{_projectDir} {
    renameAction = new ProjectItemAction{"Rename", this};
    closeAction = new ProjectItemAction("Close project", this);
}

Project::~Project() {
    delete closeAction;
    delete renameAction;
}

QVariant Project::data(int role) const {
    if (role == Qt::DisplayRole)
        return QVariant{projectDir.dirName()};
    else if (role == Qt::DecorationRole)
        return QVariant{QFileIconProvider{}.icon(QFileIconProvider::Folder)};
    else
        return QVariant{};
}

QString Project::path() const noexcept {
    return projectDir.absolutePath();
}

QList<ProjectItemAction*> Project::removeActions() const {
    QList<ProjectItemAction*> menuActions;
    menuActions.append(closeAction);
    return menuActions;
}

QList<ProjectItemAction*> Project::changeDataActions() const {
    QList<ProjectItemAction*> actions;
    actions.append(renameAction);
    return actions;
}

bool Project::handleRemoveAction(ProjectItemAction* action) {
    bool actionHandled = false;

    if (action == closeAction) {
        auto buttonPressed = QMessageBox::question(0, "Closing Project", QString("Are you sure you want to close the project `%0`?").arg(data().toString()));
        if (buttonPressed == QMessageBox::Yes) {
            actionHandled = true;
        }
    }

    return actionHandled;
}

bool Project::handleChangeDataAction(ProjectItemAction* action) {
    bool actionHandled = false;

    if (action == renameAction) {
        QString newName = QInputDialog::getText(0, "Rename Directory", "New name: ", QLineEdit::Normal, projectDir.dirName());
        if (!newName.isEmpty()) {
            QString oldName = projectDir.dirName();
            projectDir.cdUp();
            actionHandled = projectDir.rename(oldName, newName);
            if (actionHandled)
                projectDir.cd(newName);
            else
                projectDir.cd(oldName);
        }
    }

    return actionHandled;
}



//~ProjectListRoot implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectListRoot::ProjectListRoot() {}

QVariant ProjectListRoot::data(int role) const {
    if (role == Qt::DisplayRole)
        return QVariant{QString{"Projects"}};
    else
        return QVariant{};
}

ProjectListItem::ChildList ProjectListRoot::loadChildren() {
    return ChildList{};
}

ProjectListItem::ChildList ProjectListRoot::loadProjects(const QList<QString>& projectDirs) {
    ChildList children;
    foreach(const QString& path, projectDirs) {
        QFileInfo pathInfo(path);
        if (pathInfo.isDir()) {
            children.push_back(std::make_unique<Project>(QDir(pathInfo.absoluteFilePath())));
        }
    }
    return children;
}

ProjectListItem::ChildPtr ProjectListRoot::loadProject(const QString& projectPath) {
    QFileInfo pathInfo(projectPath);
    if (pathInfo.isDir()) {
        return std::make_unique<Project>(QDir(pathInfo.absoluteFilePath()));
    }
    else {
        return ChildPtr{nullptr};
    }
}
