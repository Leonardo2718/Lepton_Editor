/*
Project: Lepton Editor
File: projecttreeitem.cpp
Author: Leonardo Banderali
Created: April 5, 2015
Last Modified: May 4, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTreeItem class represents a single item in the Lepton projects tree structure.

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

#include "projecttreeitem.h"

// include Qt classes
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>



//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTreeItem::ProjectTreeItem(const QVariantMap& _data, ProjectTreeItem* _parent) : QObject(_parent), parent(_parent), data(_data) {
    children.clear();
    contextMenuActions = new QActionGroup(0);
    //connect(contextMenuActions, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuActionTriggered(QAction*)));
}

ProjectTreeItem::~ProjectTreeItem(){
    clear();
    delete contextMenuActions;
}



//~public getters and setters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const ProjectTreeItem* ProjectTreeItem::getParent() const {
    return parent;
}

/*
returns pointer to the child with a given index
*/
const ProjectTreeItem* ProjectTreeItem::getChild(int index) const {
    if (index < 0 || index >= children.length())
        return 0;
    else
        return children.at(index);
}

/*
returns internal index of a child
*/
int ProjectTreeItem::getChildIndex(const ProjectTreeItem* child) const {
    return children.indexOf((ProjectTreeItem*)child);
}

const QVariantMap& ProjectTreeItem::getData() const {
    return data;
}

/*
returns the datum associated with a given key
*/
QVariant ProjectTreeItem::getDataItem(const QString& key) const {
    return data.value(key, QVariant());
}



//~other public functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool ProjectTreeItem::hasChildren() const {
    return !children.isEmpty();
}

int ProjectTreeItem::childCount() const {
    return children.length();
}

const ProjectTreeItem* ProjectTreeItem::addChild(const QVariantMap& _data) {
    ProjectTreeItem* newChild = new ProjectTreeItem(_data, this);
    children.append(newChild);
    return newChild;
}

/*
removes a specific child
*/
bool ProjectTreeItem::removedChild(ProjectTreeItem* child) {
    return children.removeOne(child);
}

void ProjectTreeItem::addContextMenuAction(QAction* a) {
    contextMenuActions->addAction(a);
}

QActionGroup* ProjectTreeItem::getContextMenuActions() {
    return contextMenuActions;
}



//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
load contents of the current project
*/
void ProjectTreeItem::reloadProject() {
    /*ProjectTreeItem* p = this;
    while(p->getParent()->getParent() != 0)
        p = (ProjectTreeItem*)p->getParent();
    p->reloadProject();*/
    parent->reloadProject();
}



//~protected functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
removes all children
*/
void ProjectTreeItem::clear() {
    qDeleteAll(children);
    children.clear();
    foreach (QAction* a, contextMenuActions->actions()) {
        contextMenuActions->removeAction(a);
        delete a;
    }
}



//~protected slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectTreeItem::contextMenuActionTriggered(QAction* actionTriggered) {
    QString actionData = actionTriggered->data().toString();
    const bool isDir = data.value("is_directory").toBool();
    const bool isFile = data.value("is_file").toBool();
    const QString path = data.value("path").toString();

    if (actionData == "%ADD_FILE" && isDir){
        QString fileName = QFileDialog::getSaveFileName(0, "New File", path);
        if (!fileName.isEmpty()) {
            QFile newFile(fileName);
            newFile.open(QFile::ReadWrite);
            newFile.close();
            reloadProject();
        }
    } else if (actionData == "%ADD_DIRECTORY" && isDir) {
        QString dirName = QFileDialog::getSaveFileName(0, "New Directory", path, QString(),0,QFileDialog::ShowDirsOnly);
        if (!dirName.isEmpty()) {
            QDir dir(path);
            dir.mkpath(dirName);
            reloadProject();
        }
    } else if (actionData == "%RENAME_DIR" && isDir) {
        QString newName = QInputDialog::getText(0, "Rename Directory", tr("Change directory name from \"%0\" to:").arg(data.value("name").toString()));
        if (!newName.isEmpty()) {
            QDir dir(path);
            dir.cdUp();
            dir.rename(data.value("name").toString(), newName);
            reloadProject();
        }
    } else if (actionData == "%REMOVE_DIR" && isDir) {
    } else if (actionData == "%OPEN_FILE" && isFile) {
    } else if (actionData == "%RENAME_FILE" && isFile) {
        QString newName = QInputDialog::getText(0, "Rename File", tr("Change file name from \"%0\" to:").arg(data.value("name").toString()));
        if (!newName.isEmpty()) {
            QDir dir = QFileInfo(path).absoluteDir();
            dir.rename(data.value("name").toString(), newName);
            reloadProject();
        }
    } else if (actionData == "%DELETE_FILE" && isFile) {
    } else if (actionData == "%REFRESH_PROJECT") {
        reloadProject();
    } else {
    }
}

