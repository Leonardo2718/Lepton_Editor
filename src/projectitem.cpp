/*
Project: Lepton Editor
File: projectitem.cpp
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: June 11, 2014

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

#include "projectitem.h"
#include <QDebug>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectItem::ProjectItem(const QList<QVariant>& data, ProjectItem* parent) {
    parentItem = parent;
    itemDisplayData = data;
}

ProjectItem::ProjectItem(const QString& itemPath, ProjectItem *parent) {
    parentItem = parent;
    item.setFile(itemPath);
    qDebug() << item.exists() << item.isDir() << item.fileName();
    if ( ! item.exists() ) itemDisplayData << QString("Path '%1' was not found!").arg(itemPath);
    else itemDisplayData << item.fileName();
}

ProjectItem::~ProjectItem() {
    qDeleteAll(children);
}

void ProjectItem::appendChild(ProjectItem* item) {
/* -adds a child item to this item */
    item->setParent(this);
    children.append(item);
}

void ProjectItem::appendChild(const QString itemPath) {
/* -adds a child item to this item */
    ProjectItem* newItem = new ProjectItem(itemPath, this);
    children.append(newItem);
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



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectItem::setParent(ProjectItem* newParent) {
/* -set new parent item of this item */
    parentItem = newParent;
}
