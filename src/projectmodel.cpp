/*
Project: Lepton Editor
File: projectmodel.cpp
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: June 11, 2014

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
#include <QDir>
#include <QFileInfoList>
#include <QString>
#include <QDebug>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectModel::ProjectModel(QObject* parent) : QAbstractItemModel(parent) {
    //create a new root item
    QList< QVariant > rootData;
    rootData << "Projects";
    rootProjectItem = new ProjectItem(rootData);

    //rootProjectItem->appendChild(someData);
    //ProjectItem* newItem = new ProjectItem("/home/leonardo/Programming/Lepton_Editor", rootProjectItem);
    //rootProjectItem->appendChild( newItem );
    /*ProjectRootItem* root = new ProjectRootItem( someData );
    rootProjectItem->appendChild( root );
    qDebug() << root->parent();*/
    addProject("/home/leonardo/Programming/Lepton_Editor");
    addProject("../");
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

    ProjectItem* item = static_cast< ProjectItem* >( index.internalPointer() ); //get current model item
    QVariant returnData;    //data to be returned

    if (role == Qt::DisplayRole) {
        returnData = item->getDisplayData( index.column() );
    }
    else if (role == Qt::DecorationRole && index.column() == 0) {   //decoration role sets icons
        returnData = item->getDecorationIcon();
    }

    return returnData;
}

QModelIndex ProjectModel::parent(const QModelIndex& child) const {
/* -returns the parent item model index of a specified item */
    if ( !child.isValid() ) return QModelIndex();   //return empty if the index is not valid (empty)

    ProjectItem* item = static_cast< ProjectItem* >( child.internalPointer() ); //get current model item
    ProjectItem* parentItem = item->parent();                                   //get parent item from current model item

    if (item == rootProjectItem) return QModelIndex();              //return empty if current item is root (root has no parent)

    return createIndex(parentItem->currentRow(), 0, parentItem);    //create and return model index of parent item
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation, int role) const {
/* -returns data to be put in header of table (header in tree view) */
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootProjectItem->getDisplayData(section);
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



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectModel::addProject(const QString& projectPath) {
/* -builds tree for a new project and adds it to the list */
    //get directory of project
    QDir projectDir(projectPath);
    if ( ! projectDir.exists() ) return;

    //create project root
    ProjectItem* newProject = new ProjectItem(projectPath, rootProjectItem);
    rootProjectItem->appendChild( newProject );

    //lists to be used as stacks to avoid using recursion to navigate directory tree
    QList< ProjectItem* > parentItems;
    QList< QFileInfoList > dirItemInfoLists;
    QList< quint32 > dirItemIndices;

    //append initial values to lists
    parentItems.append(newProject);
    dirItemInfoLists.append( projectDir.entryInfoList() );
    dirItemIndices.append( 2 ); //indices '0' and '1' represent the current and parent directories and must therefore be ignored

    //process each file and sub-directory in the project to add them to this model
    while ( parentItems.count() > 0 || dirItemInfoLists.count() > 0 || dirItemIndices.count() > 0 ) {
    //while there are items present in the lists, do the following:
        //if the there are no items left to be added in a directory, remove item form the stacks to return to the parent directory and continue processing its contents
        if ( dirItemIndices.last() >= dirItemInfoLists.last().count() ) {
            parentItems.removeLast();
            dirItemInfoLists.removeLast();
            dirItemIndices.removeLast();
        }
        //otherwise, make sure that there are items to be processed (the first two should be ignored)
        //  Note that if this condition is false, then the previous condition must be true because the lowest value that is ever assigned as an index is '2' and there
        //  must always be a minimum of 2 directories (i.e. the current directory '.' and parent directory '..').  As a result, an infinit loop does not occure.
        else if ( dirItemInfoLists.last().count() > 2 ) {
            //create and add the new item to the tree model
            QString itemPath = dirItemInfoLists.last().at( dirItemIndices.last() ).absoluteFilePath();
            ProjectItem* newItem = new ProjectItem(itemPath, parentItems.last() );
            parentItems.last()->appendChild(newItem);

            //if the new item is a directory, add it to the stacks to process its contents
            if ( dirItemInfoLists.last().at( dirItemIndices.last() ).isDir() ) {
                dirItemIndices.last() += 1;
                QDir dir( itemPath );
                parentItems.append(newItem);
                dirItemInfoLists.append( dir.entryInfoList() );
                dirItemIndices.append( 2 );
            }
            //otherwise, just move on to the next item in the current directory
            else dirItemIndices.last() += 1;
        }
    }
}
