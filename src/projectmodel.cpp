/*
Project: Lepton Editor
File: projectmodel.cpp
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: September 3, 2014

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
#include <QFileDialog>
#include <QSettings>
#include <QVariant>

#include <QDebug>

//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectModel::ProjectModel(QObject* parent) : QAbstractItemModel(parent) {
    //create a new root item
    QList< QVariant > rootData;
    rootData << "Projects";
    rootProjectItem = new ProjectItem(rootData);

    //create actions for model items
    fileActions = new QActionGroup(this);
    fileActions->addAction("Open this file");

    dirActions = new QActionGroup(this);

    projectActions = new QActionGroup(this);
    projectActions->addAction("Close project");
    projectActions->addAction("Reload project");

    connect(fileActions, SIGNAL(triggered(QAction*)), this, SLOT(actionOnFileTriggered(QAction*)));
    connect(dirActions, SIGNAL(triggered(QAction*)), this, SLOT(actionOnDirTriggered(QAction*)));
    connect(projectActions, SIGNAL(triggered(QAction*)), this, SLOT(actionOnProjectTriggered(QAction*)));

    //load project list from previous session
    QSettings session;
    QList< QVariant > projectPathList = session.value("projectPathList").toList();
    for (int i = 0, c = projectPathList.count(); i < c; i++) {
        addProject( projectPathList.at(i).toString() );
    }
}

ProjectModel::~ProjectModel() {

    //save project list in session
    QSettings session;
    QList< ProjectItem* > projects = rootProjectItem->getChildren();
    QList< QVariant > projectPathList;
    for (int i = 0, c = projects.count(); i < c; i++) {
        projectPathList.append( projects.at(i)->getPath() );
    }
    session.setValue("projectPathList", projectPathList);

    delete fileActions;
    delete dirActions;
    delete projectActions;
    delete rootProjectItem;
}

bool ProjectModel::removeRows(int row, int count, const QModelIndex &parent) {
/* -removes a row from the model */
    ProjectItem* parentItem = static_cast< ProjectItem* >( parent.internalPointer() );  //get pointer to parent item from index
    if ( (parentItem == NULL) || (row >= parentItem->childCount()) || (row+count > parentItem->childCount()) ) return false;    //check parameter validity

    emit beginRemoveRows(parent, row, row+count);       //emit signal to notify viewing class that model is changing
    for (int i = row + count - 1; i >= row ; i--) {     //for every row to be removed
        parentItem->removeChild( parentItem->child(i) );  //remove item
    }
    emit endRemoveRows();                               //emit signal to notify viewing class that model has changed

    return true;
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
    if (!parentItem) return QModelIndex();

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

QList< QAction* > ProjectModel::getActionsFor(const QModelIndex& index) {
/* -returns all actions which can be performed on a given item */
    actionEmiter = index;
    ProjectItem* item = static_cast< ProjectItem* >( index.internalPointer() );
    if (item->parent() == rootProjectItem) return projectActions->actions(); //if item is a project directory
    else if ( item->isDirectory() ) return dirActions->actions();
    else if ( item->isFile() ) return fileActions->actions();
    else return QList< QAction* >();
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

Qt::ItemFlags ProjectModel::flags(const QModelIndex& index) const {
/* -returns the flags which correspond to the item referenced by 'index' */
    if ( !index.isValid() || index.internalPointer() == rootProjectItem) return Qt::NoItemFlags; //if index is not valid, it cannot have any flags
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role) {
/* -sets the data for an item and returns true if succesful */
    /*####################################################################################################
    ### In this case, data should not actually be changed using this method.  However, this method must ##
    ### be reimplemented in order to allow the model to change.  As a result, true is always returned   ##
    ### unless the item which should be modified does not exist.                                        ##
    ####################################################################################################*/
    if ( (!index.isValid()) || (index.internalPointer() == rootProjectItem) ) return false;
    else return true;
}

int ProjectModel::projectCount() {
/*  -return the number of projects in the model */

    return rootProjectItem->childCount();
}



//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectModel::openProjectRequest() {
/* -called by user to open a project */
    QString dirPath = QFileDialog::getExistingDirectory(0, tr("Open Project Directory"), QDir::homePath());
    if ( dirPath.isEmpty() ) return;
    QDir dir(dirPath);
    addProject( dir.absolutePath() );
}

void ProjectModel::newProjectRequest() {
/* -called by user to create a new project */
    ProjectCreatorDialog projectCreator;   //project creation dialog box
    int result = projectCreator.exec();
    if (result != QDialog::Accepted) return;
    QDir pDir( projectCreator.getProjectPath() );
    if ( ! pDir.exists() ) return;
    QString pName = projectCreator.getProjectName();
    if ( pName.isEmpty() ) return;
    bool newDirMade = pDir.mkdir(pName);
    if ( ! newDirMade ) return;
    addProject( pDir.absoluteFilePath(pName) );
}



//~private slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectModel::actionOnFileTriggered(QAction* action) {
/* -called when action from project file context menu is triggered */
    //get the emiting item
    if ( ! actionEmiter.isValid() ) return;
    ProjectItem* emitingItem = static_cast< ProjectItem* >( actionEmiter.internalPointer() );

    //emit signal which corresponds to action
    if (action->text() == "Open this file"){
        emit openFileRequested( emitingItem->getPath() );
    }
}

void ProjectModel::actionOnDirTriggered(QAction* action) {
/* -called when action from project directory context menu is triggered */
}

void ProjectModel::actionOnProjectTriggered(QAction* action) {
/* -called when action from project context menu is triggered */
    //get the emiting item
    if ( (! actionEmiter.isValid()) || (! hasChildren(actionEmiter)) || (!(actionEmiter.parent().internalPointer() == rootProjectItem)) ) return;
    ProjectItem* emitingItem = static_cast< ProjectItem* >( actionEmiter.internalPointer() );

    //emit signal which corresponds to action
    if (action->text() == "Close project") {
        removeRows( actionEmiter.row(), 1, actionEmiter.parent() );
    }
    else if (action->text() == "Reload project") {
        emit layoutAboutToBeChanged();                          //emit change signal
        removeRows(0, emitingItem->childCount(), actionEmiter); //remove everything from the project
        populateProject(emitingItem);                           //re-populate the project by going through its directory
        emit layoutChanged();
    }

    return;
}



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ProjectModel::addProject(const QString& projectPath) {
/* -builds tree for a new project and adds it to the list */
    //get directory of project
    QDir projectDir(projectPath);
    if ( ! projectDir.exists() ) return;

    emit layoutAboutToBeChanged();  //emit signal to notify viewing class that model is changing

    //create project root
    ProjectItem* newProject = new ProjectItem(projectPath, rootProjectItem);
    rootProjectItem->appendChild( newProject );
    populateProject(newProject);

    emit layoutChanged();           //emit signal to notify viewing class that model has changed
}

void ProjectModel::populateProject(ProjectItem* projectRoot) {
/* -populates a project item with its child items */
    if ( !(projectRoot->parent() == rootProjectItem) || !(projectRoot->isDirectory()) ) return;

    //get project directory
    QDir projectDir( projectRoot->getPath() );

    //lists to be used as stacks to avoid using recursion to navigate directory tree
    QList< ProjectItem* > parentItems;
    QList< QFileInfoList > dirItemInfoLists;
    QList< quint32 > dirItemIndices;

    //append initial values to lists
    parentItems.append(projectRoot);
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



//~'ProjectCreatorDialog' class~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectCreatorDialog::ProjectCreatorDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f) {

    mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(30);
    this->setLayout(mainLayout);
    this->setWindowTitle("Lepton Project Creator");

    creatorFormLayout = new QGridLayout(this);
    creatorFormLayout->setVerticalSpacing(5);
    creatorFormLayout->setHorizontalSpacing(5);

    projectNameField = new QLineEdit(this);
    labels.append( new QLabel("Project Name: ", this) );
    creatorFormLayout->addWidget(labels[0], 0, 0);
    creatorFormLayout->addWidget(projectNameField, 0, 1);

    projectPathField = new QLineEdit(this);
    labels.append( new QLabel("Project Path: ", this) );
    directoryBrowse = new QPushButton("&Browse", this);
    connect(directoryBrowse, SIGNAL(clicked()), this, SLOT(browseClicked()) );
    creatorFormLayout->addWidget(labels[1], 1, 0);
    creatorFormLayout->addWidget(projectPathField, 1, 1);
    creatorFormLayout->addWidget(directoryBrowse, 1, 2);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help, Qt::Vertical, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    mainLayout->addItem(creatorFormLayout);
    mainLayout->addWidget(buttonBox);
}

ProjectCreatorDialog::~ProjectCreatorDialog() {
    labels.clear();
    disconnect(directoryBrowse, SIGNAL(clicked()), this, SLOT(browseClicked()) );
    disconnect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    disconnect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ProjectCreatorDialog::getProjectName() {
/* -returns entered project name */
    return projectName;
}

QString ProjectCreatorDialog::getProjectPath() {
/* -returns entered project path */
    return projectPath;
}

void ProjectCreatorDialog::browseClicked() {
    QString path = QFileDialog::getExistingDirectory(0, tr("Project Path"), QDir::homePath());
    if ( path.isEmpty() ) return;
    QDir dir(path);
    if ( ! dir.exists() ) return;
    projectPathField->setText(dir.absolutePath());
}

void ProjectCreatorDialog::accept() {
    if ( projectNameField->text().isEmpty() || projectPathField->text().isEmpty() ) return;
    projectName = projectNameField->text();
    projectPath = projectPathField->text();
    QDialog::accept();
}

void ProjectCreatorDialog::reject() {
    QDialog::reject();
}
