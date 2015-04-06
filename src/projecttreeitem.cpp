/*
Project: Lepton Editor
File: projecttreeitem.cpp
Author: Leonardo Banderali
Created: April 5, 2015
Last Modified: April 5, 2015

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



//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTreeItem::ProjectTreeItem(const QVariantMap& _data, ProjectTreeItem* _parent) : QObject(_parent), parent(_parent), data(_data) {
    children.clear();
    contextMenuActions = new QActionGroup(0);
    connect(contextMenuActions, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuActionTriggered(QAction*)));
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

void ProjectTreeItem::addContextMenuAction(QAction* a) {
    contextMenuActions->addAction(a);
}

QList<QAction*> ProjectTreeItem::getContextMenuActions() {
    return contextMenuActions->actions();
}


//~protected slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectTreeItem::contextMenuActionTriggered(QAction* actionTriggered) {
    QString data = actionTriggered->data().toString();
    if (data == "%ADD_FILE"){
    } else if (data == "%ADD_DIRECTORY") {
    } else if (data == "%REFRESH_PROJECT") {
    } else if (data == "%RENAME_DIR") {
    } else if (data == "%REMOVE_DIR") {
    } else if (data == "%OPEN_FILE") {
    } else if (data == "%RENAME_FILE") {
    } else if (data == "%DELETE_FILE") {
    } else {
    }
}

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

