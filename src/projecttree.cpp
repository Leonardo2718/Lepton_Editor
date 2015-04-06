/*
Project: Lepton Editor
File: projecttree.cpp
Author: Leonardo Banderali
Created: April 5, 2015
Last Modified: April 5, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTree class is represents the root item for the projects tree.  All direct
    children of this root class are actual Lepton projects.  This class is used to facilitate
    project management.

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

#include "projecttree.h"

// include Qt classes
#include <QFileDialog>
#include <QDir>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTree::ProjectTree() : ProjectTreeItem(QVariantMap(), 0) {
    LeptonProject* p = new LeptonProject(this, "/home/leonardo/Programming/Lepton_Editor/build-Lepton-Desktop-Debug/test_project");
    children.append(p);
}

ProjectTree::~ProjectTree() {
    qDeleteAll(children);
}

/*
returns pointer to the child with a given index
*/const ProjectTreeItem* ProjectTree::getChild(int index) const {
    return children.at(index);
}

/*
returns internal index of a child
*/
int ProjectTree::getChildIndex(const ProjectTreeItem* child) const {
    return children.indexOf((LeptonProject*)child);
}

bool ProjectTree::hasChildren() const {
    return !children.isEmpty();
}

int ProjectTree::childCount() const {
    return children.count();
}

bool ProjectTree::removedChild(ProjectTreeItem* child) {
    return children.removeOne((LeptonProject*)child);
}



//~other public functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
opens dialog to create a new project
*/
void ProjectTree::createNewProject() {
    QString dirName = QFileDialog::getSaveFileName(0, "New Project", QDir::homePath(), QString(),0,QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty() && QDir::home().mkdir(dirName))
        addChild(dirName);
}

/*
opens dialog to open an already existing project
*/
void ProjectTree::openProject() {
    QString dirName = QFileDialog::getExistingDirectory(0, "Open Project", QDir::homePath(), QFileDialog::ShowDirsOnly);
    if (!dirName.isEmpty())
        addChild(dirName);
}



//~private functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const ProjectTreeItem* ProjectTree::addChild(const QString& dirName) {
    LeptonProject* p = new LeptonProject(this, dirName);
    children.append(p);
}

