/*
Project: Lepton Editor
File: projectitem.cpp
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: June 10, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The 'ProjectItem' class is intended to represent one item in a project model.  This may be
    a directory or a file which is part of a project.  The ProjectRootItem represents the root
    directory of a project.

Copyright (C) 2014 Leonardo Banderali

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

#include "projectitem.h"
#include <QDebug>



//~public methods of 'ProjectItem' class~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectItem::ProjectItem(const QList< QVariant > &data, ProjectItem* parent) {
    itemData = data;
    parentItem = parent;
}

ProjectItem::~ProjectItem() {
    qDeleteAll(children);
}

void ProjectItem::appendChild(ProjectItem* item) {
/* -adds a child item to this item */
    item->setParent(this);
    children.append(item);
}

void ProjectItem::appendChild(const QList< QVariant > data) {
/* -adds a child item to this item */
    ProjectItem* newItem = new ProjectItem(data, this);
    children.append(newItem);
}

ProjectItem* ProjectItem::child(int row) {
/* -returns child item at 'row' ('row' is index of item) */

    return children.value(row, NULL);    //will return default item (null pointer) if row/index is not valid
}

QVariant ProjectItem::data(int column) const {
/* -returns data element of this item with index 'column' */
    return itemData.value(column, QVariant() );
}

ProjectItem* ProjectItem::parent() {
/* -returns pointer to parent item (null if this item is root) */
    return parentItem;
}

int ProjectItem::childCount() const {
/* -returns number of child items */
    return children.length();
}

int ProjectItem::columnCount() const {
/* -returns number of data elements */
    return itemData.length();
}

int ProjectItem::dataElementCount() const {
/* -returns number of data elements (same as 'columnCount()' ) */
    return itemData.length();
}

int ProjectItem::row() const {
/* -returns the row (index) of this item in its parent ('0' if this item is root) */
    if (parentItem == NULL) return 0;
    return parentItem->children.indexOf( const_cast< ProjectItem* >(this) );
}



//~public methods of 'ProjectRootItem' class~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectRootItem::ProjectRootItem(const QList< QVariant >& rootData) : ProjectItem(rootData, 0) {

}

ProjectRootItem::~ProjectRootItem() {

}



//~private methods of 'ProjectItem' class~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectItem::setParent(ProjectItem* newParent) {
/* -set new parent item of this item */
    parentItem = newParent;
}
