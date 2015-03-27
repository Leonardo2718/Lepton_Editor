/*
Project: Lepton Editor
File: leptonprojectitem.cpp
Author: Leonardo Banderali
Created: March 23, 2015
Last Modified: March 24, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The LeptonProjectItem class represents a single project item in any Lepton project.

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

#include "leptonprojectitem.h"



//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonProjectItem::LeptonProjectItem(const QString& _name, const QString& _type, LeptonProjectItem* _parent) : QObject(0) {
    name = _name;
    type = _type;
    projectParent = _parent;
    children.clear();
    contextMenuActions = new QActionGroup(0);
    connect(contextMenuActions, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuActionTriggered(QAction*)));
}

LeptonProjectItem::LeptonProjectItem() : QObject(0) {   // hide default constructor from outside classes but let subclasses use it
    children.clear();
    contextMenuActions = new QActionGroup(0);
}

LeptonProjectItem::~LeptonProjectItem(){
    clear();
    projectParent = 0;
}



//~getters and setters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QString LeptonProjectItem::getName() const {
    return name;
}

QString LeptonProjectItem::getType() const {
    return type;
}

QList<QAction*> LeptonProjectItem::getActions() {
    return contextMenuActions->actions();
}

const LeptonProjectItem* LeptonProjectItem::getParent() const {
    return projectParent;
}



//~other public functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool LeptonProjectItem::hasChildren() const {
    return !children.isEmpty();
}

int LeptonProjectItem::childCount() const {
    return children.length();
}

int LeptonProjectItem::getChildIndex(LeptonProjectItem* child) const {
    if (children.isEmpty())
        return -1;
    else
        return children.indexOf(child);
}

const LeptonProjectItem* LeptonProjectItem::addChild(const QString& _name, const QString& _type) {
    LeptonProjectItem* i = new LeptonProjectItem(_name, _type, this);
    children.append(i);
    return i;
}

const LeptonProjectItem* LeptonProjectItem::getChild(int index) const {
    if (index < 0 || index >= children.length())
        return 0;
    else
        return children[index];
}

void LeptonProjectItem::addAction(QAction* a) {
    contextMenuActions->addAction(a);
}



//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LeptonProjectItem::contextMenuActionTriggered(QAction* actionTriggered) {
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

