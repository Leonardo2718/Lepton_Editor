/*
Project: Lepton Editor
File: projectitem.cpp
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: August 25, 2014

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

#include <QFileIconProvider>
#include <QDir>

#include "projectitem.h"



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectItem::ProjectItem(const QList<QVariant>& data, ProjectItem* parent) {
    parentItem = parent;
    itemDisplayData = data;
}

ProjectItem::ProjectItem(const QString& itemPath, ProjectItem *parent) {
    parentItem = parent;
    item.setFile(itemPath);
    item.makeAbsolute();
    if ( ! item.exists() ) itemDisplayData << QString("Path '%1' was not found!").arg(itemPath);
    else itemDisplayData << item.fileName();
}

ProjectItem::~ProjectItem() {
    //parentItem = NULL;
    qDeleteAll(children);
    children.clear();
}

void ProjectItem::appendChild(ProjectItem* newItem) {
/* -adds a child item to this item */
    newItem->setParent(this);
    children.append(newItem);
}

void ProjectItem::appendChild(const QString itemPath) {
/* -adds a child item to this item */
    ProjectItem* newItem = new ProjectItem(itemPath, this);
    children.append(newItem);
}

void ProjectItem::removeChild(ProjectItem* child) {
/* -removes a child form this item */
    int childIndex = children.indexOf(child);                           //get the index of the child
    if ( !(item.isDir() || item.isRoot()) || childIndex < 0) return;    //check that the child exists
    delete children[childIndex];                                            //remove child form memory
    children.removeAt(childIndex);
}

ProjectItem* ProjectItem::child(int index) {
/* -returns child item at 'index' */

    return children.value(index, NULL);    //will return default item (null pointer) if row/index is not valid
}

QVariant ProjectItem::getDisplayData(int column) const {
/* -returns data element of this item with index 'column' */
    return itemDisplayData.value(column, QVariant() );
}

QIcon ProjectItem::getDecorationIcon() const {
/* -returns icon to be displayed next to the item display data in a view */
    QFileIconProvider iconProvide;
    if ( item.exists() ) return iconProvide.icon(item);
    return QIcon();
}

ProjectItem* ProjectItem::parent() {
/* -returns pointer to parent item (null if this item is root) */
    return parentItem;
}

QString ProjectItem::getPath() const {
/* -returns path to this item */
    return item.absoluteFilePath();
}

int ProjectItem::childCount() const {
/* -returns number of child items */
    return children.length();
}

int ProjectItem::columnCount() const {
/* -returns number of data elements */
    return itemDisplayData.length();
}

int ProjectItem::dataElementCount() const {
/* -returns number of data elements (same as 'columnCount()' ) */
    return itemDisplayData.length();
}

int ProjectItem::currentIndex() const {
/* -returns the index of this item in its parent ('0' if this item is root) */
    if (parentItem == NULL) return 0;
    return parentItem->children.indexOf( const_cast< ProjectItem* >(this) );
}

int ProjectItem::currentRow() const {
/* -returns the row (index) of this item (same as 'currentIndex()') */
    if (parentItem == NULL) return 0;
    return parentItem->children.indexOf( const_cast< ProjectItem* >(this) );
}

bool ProjectItem::isFile() const {
/* -returns whether this item is a file */
    return (item.exists() && item.isFile());
}

bool ProjectItem::isDirectory() const {
/* -returns whether this item is a directory */
    return (item.exists() && item.isDir());
}



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectItem::setParent(ProjectItem* newParent) {
/* -set new parent item of this item */
    parentItem = newParent;
}
