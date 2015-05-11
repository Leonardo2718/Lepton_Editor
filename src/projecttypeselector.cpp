/*
Project: Lepton Editor
File: projecttypeselector.cpp
Author: Leonardo Banderali
Created: May 11, 2015
Last Modified: May 11, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTypeSelector is a widget used for graphically selecting a project type
    from the project specs directory. The selected type can then be used to create a
    new project or open an existing one using the selected project spec.

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

#include "projecttypeselector.h"
#include "ui_projecttypeselector.h"



//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTypeSelector::ProjectTypeSelector(QWidget *parent) : QWidget(parent), ui(new Ui::ProjectTypeSelector), selectedItem() {
    ui->setupUi(this);
    projectTypes = new ProjectTypeModel();
    ui->projectTypeDisplay->setModel(projectTypes);
    connect(ui->projectTypeDisplay, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
    connect(ui->projectTypeDisplay, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleClicked(QModelIndex)));
}

ProjectTypeSelector::~ProjectTypeSelector() {
    delete projectTypes;
    delete ui;
}



//~other public functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-returns the file path to the spec file selected
*/
QString ProjectTypeSelector::selectedSpecFile() const {
    return projectTypes->specFileFromIndex(selectedItem);
}



//~private slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
handles an item being clicked
*/
void ProjectTypeSelector::itemClicked(const QModelIndex& index) {
    selectedItem = index;
    emit itemSelected();
}

/*
handles an item being double clicked
*/
void ProjectTypeSelector::itemDoubleClicked(const QModelIndex& index) {
    selectedItem = index;
    emit itemChosen();
}

