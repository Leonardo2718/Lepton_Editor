/*
Project: Lepton Editor
File: projectlistitem.cpp
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 18, 2015

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
Constructs a new item/node and adds it to the child list. To construct the item, the function
`constructChild(args)`. If the child returned is null (nullptr), then nothing else will happen
and the function will return false. So, code to cancel an addition can be implemented there.
True will be returned if the child was constructed and added successfully, false otherwise.
*/
/*bool ProjectListItem::addChild(const QVariantList& args) {
    auto newChild = this->constructChild(args);
    if (newChild) {
        newChild->parentPtr = this;
        children.push_back(std::move(newChild));
        return true;
    }
    else {
        return false;
    }
}*/

void ProjectListItem::addChild(std::unique_ptr<ProjectListItem> newChild) {
    newChild->parentPtr = this;
    children.push_back(std::move(newChild));
}

/*
Removes the child at `index`. Before removing the child, `cleanup()` will be called on that child to
perform any side-effects required and do some cleanup. The child will only be removed if this call
returns true. So, code to cancel a removal can be implemented there. True will be returned if the
child was removed successfully, false otherwise.
*/
/*bool ProjectListItem::removeChild(int index) {
    if (children[index]->cleanup()) {
        return true;
    }
    else {
        return false;
    }
}*/

std::unique_ptr<ProjectListItem> ProjectListItem::removeChild(ProjectListItem* child) {
    child->parentPtr = nullptr;
    auto itr = children.cbegin();
    for (; itr != children.cend(); itr++) {
        if (itr->get() == child) {
            break;
        }
    }
    if (itr != children.cend()) {
        auto oldChild = std::move(children[itr - children.cbegin()]);
        children.erase(itr);
        return oldChild;
    }
    else {
        return std::unique_ptr<ProjectListItem>{nullptr};
    }
}

/*
Returns the actions for the context menu to be displayed when this item is right-clicked in the
project manager. Any action within this group must store as its data a pointer to the item it
belongs to.
*/
QList<ProjectItemAction*> ProjectListItem::contextMenuActions() const {
    return QList<ProjectItemAction*>{};
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
bool ProjectListItem::handleNewChildAction(ProjectItemAction* newChildAction) {
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

ProjectFile::ProjectFile(const QFileInfo& _file) : file{_file} {}

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

QList<ProjectListItem*> ProjectFile::loadChildren() {
    return QList<ProjectListItem*>{};
}



//~ProjecDirectory implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectDirectory::ProjectDirectory(const QDir& _dir) : dir{_dir} {}

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

QList<ProjectListItem*> ProjectDirectory::loadChildren() {
    QList<ProjectListItem*> children;
    foreach (QFileInfo itemInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        if (itemInfo.isDir()) {
            std::unique_ptr<ProjectListItem> child = std::make_unique<ProjectDirectory>(QDir(itemInfo.absoluteFilePath()));
            children.append(child.get());
            addChild(std::move(child));
        }
        else if (itemInfo.isFile()) {
            std::unique_ptr<ProjectListItem> child = std::make_unique<ProjectFile>(itemInfo);
            children.append(child.get());
            addChild(std::move(child));
        }
    }
    return children;
}



//~Project implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Project::Project(const QDir& _projectDir) : ProjectDirectory{_projectDir}, projectDir{_projectDir} {
    /*QAction* closeProjectAction = new QAction("Close Project", 0);
    connect(closeProjectAction, SIGNAL(triggered(bool)), this, SLOT(handleCloseProject(bool)));
    menuActions.append(closeProjectAction);*/
    closeAction = new ProjectItemAction("Close Project", this);
}

Project::~Project() {
    //qDeleteAll(menuActions);
    delete closeAction;
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

QList<ProjectItemAction*> Project::contextMenuActions() const {
    QList<ProjectItemAction*> menuActions;
    menuActions.append(closeAction);
    return menuActions;
}

QList<ProjectItemAction*> Project::removeActions() const {
    QList<ProjectItemAction*> menuActions;
    menuActions.append(closeAction);
    return menuActions;
}

bool Project::handleRemoveAction(ProjectItemAction* action) {
    //bool actionHandled = false;

    if (action == this->closeAction) {
        auto buttonPressed = QMessageBox::question(0, "Closing Project", QString("Are you sure you want to close the project `%0`?").arg(data().toString()));
        qDebug() << buttonPressed;
        if (buttonPressed == QMessageBox::Yes) {
            auto self = parent()->removeChild(this);    // causes `this` to be deleted at end of block because of scope exit!
            //actionHandled = true;
            return true;                                // return now to avoid UB because `this` had been deleted
        }
    }

    //return actionHandled;
    return false;
}



//~ProjectListRoot implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectListRoot::ProjectListRoot() {}

QVariant ProjectListRoot::data(int role) const {
    if (role == Qt::DisplayRole)
        return QVariant{QString{"Projects"}};
    else
        return QVariant{};
}

QList<ProjectListItem*> ProjectListRoot::loadChildren() {
    return QList<ProjectListItem*>{};
}

QList<ProjectListItem*> ProjectListRoot::loadProjects(const QList<QString>& projectDirs) {
    QList<ProjectListItem*> children;
    foreach(const QString& path, projectDirs) {
        QFileInfo pathInfo(path);
        if (pathInfo.isDir()) {
            std::unique_ptr<ProjectListItem> child = std::make_unique<Project>(QDir(pathInfo.absoluteFilePath()));
            children.append(child.get());
            addChild(std::move(child));
        }
    }
    return children;
}

ProjectListItem* ProjectListRoot::loadProject(const QString& projectPath) {
    ProjectListItem* newProject = nullptr;
    QFileInfo pathInfo(projectPath);
    if (pathInfo.isDir()) {
        std::unique_ptr<ProjectListItem> child = std::make_unique<Project>(QDir(pathInfo.absoluteFilePath()));
        newProject = child.get();
        addChild(std::move(child));
    }
    return newProject;
}
