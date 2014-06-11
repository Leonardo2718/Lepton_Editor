/*
Project: Lepton Editor
File: projectmodel.cpp
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: June 10, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectModel class is intended to be a data model class to represent a directory or group of files
    which together form a programming project.  This class is used to access and manipulate source files at
    the project level.

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

#include "projectmodel.h"

#include <QList>
#include <QDebug>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectModel::ProjectModel(QObject* parent) : QAbstractItemModel(parent) {
    //create a new root item
    QList< QVariant > rootData;
    rootData << "Project" << "More Stuff" << "Even more";
    rootProjectItem = new ProjectItem(rootData);

    QList< QVariant > someData;
    someData << "someThing" << "some thing else" << "I don't know what to write";
    rootProjectItem->appendChild(someData);
    ProjectItem* newItem = rootProjectItem->child(0);
    newItem->appendChild( someData );
    ProjectRootItem* root = new ProjectRootItem( someData );
    qDebug() << root->parent();
}

ProjectModel::~ProjectModel() {
    delete rootProjectItem;
}

QModelIndex ProjectModel::index(int row, int column, const QModelIndex& parent) const {
/*
-returns the model index of an item specified by its 'row' and 'column' numbers and its parent item
-if the item is not in the model, an empty index is returned
*/
    if ( !hasIndex(row, column, parent) ) return QModelIndex(); //if the model item does not exist return an empty index

    ProjectItem* parentItem;    //the parent of the item of which we need the index (referenced by index 'parent')
    ProjectItem* childItem;     //the item whose index we are looking for

    if ( !parent.isValid() ) parentItem = rootProjectItem;  //if the parent index is empty, then the parent item must be root
    else parentItem = static_cast< ProjectItem* >( parent.internalPointer() );

    childItem = parentItem->child(row); //get the child form the parent

    if (childItem) return createIndex(row, column, childItem);  //if the child element exists, create and return a model index for it
    else return QModelIndex();                                  //otherwise return an empty index
}

QVariant ProjectModel::data(const QModelIndex& index, int role) const {
/* -returns data associated with an item stored under a given 'role' */
    if ( !index.isValid() ) return QVariant();  //return empty if the index is not valid (empty)

    if (role == Qt::DisplayRole) {
        ProjectItem* item = static_cast< ProjectItem* >( index.internalPointer() ); //get current model item
        return item->data( index.column() );    //return item data
    }
    else {                  //if the 'role' could not be handled return empty
        return QVariant();
    }
}

QModelIndex ProjectModel::parent(const QModelIndex& child) const {
/* -returns the parent item model index of a specified item */
    if ( !child.isValid() ) return QModelIndex();   //return empty if the index is not valid (empty)

    ProjectItem* item = static_cast< ProjectItem* >( child.internalPointer() ); //get current model item
    ProjectItem* parentItem = item->parent();                                   //get parent item from current model item

    if (item == rootProjectItem) return QModelIndex();  //return empty if current item is root (root has no parent)

    return createIndex(parentItem->row(), 0, parentItem);   //create and return model index of parent item
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation, int role) const {
/* -returns data to be put in header of table (header in tree view) */
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootProjectItem->data(section);
    }
    else return QVariant();
}

int ProjectModel::rowCount(const QModelIndex& parent) const {
/* -returns the number of rows in a model item */
    if ( parent.column() > 0 ) return 0;    //child items can only be in the first column (column 0)

    ProjectItem* parentItem;
    if ( parent.isValid() ) parentItem = static_cast< ProjectItem* >( parent.internalPointer() );   //if the model index is not valid (is empty) then the parent must be root
    else parentItem = rootProjectItem;

    return parentItem->childCount();
}

int ProjectModel::columnCount(const QModelIndex& parent) const {
/* -returns the number of columns in a model item */
    ProjectItem* parentItem;
    if ( parent.isValid() ) parentItem = static_cast< ProjectItem* >( parent.internalPointer() );   //if the model index is not valid (is empty) then the parent must be root
    else parentItem = rootProjectItem;

    return parentItem->columnCount();
}

/*Qt::ItemFlag ProjectModel::flags(const QModelIndex& index) const {
* -returns the flags which correspond to the item referenced by 'index' *
    if ( !index.isValid() ) return Qt::NoItemFlags; //if index is not valid, it cannot have any flags
    return QAbstractItemModel::flags(index);
}*/
