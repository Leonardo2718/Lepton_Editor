/*
Project: Lepton Editor
File: projectlistitem.cpp
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 11, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectListItem class represents an item in the project list. Specializations
    of this class are used to implement different behavior for different types of items.

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

// project headers
#include "projectlistitem.h"

// Qt classes
#include <QMessageBox>
#include <QDir>
#include <QString>



//~ProjectListItem implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectListItem::~ProjectListItem() noexcept = default;

ProjectListItem* ProjectListItem::parent() noexcept {
    return parentPtr;
}

int ProjectListItem::childCount() const noexcept {
    return children.size();
}

ProjectListItem* ProjectListItem::childAt(int row) noexcept {
    return children[row].get();
}

int ProjectListItem::indexOfChild(ProjectListItem* child) const noexcept {

    // perform linear search to find the index of the child (see note bellow)
    for (int i = 0, count = children.size(); i < count; i++) {
        if (children[i].get() == child) {
            return i;
        }
    }

    /*#############################################################################################
    ### Although it is usually considered bad practice to reimplement such a common algorithm    ##
    ### (here, the `QList::indexOf()` function does what we want), I did so in this case because ##
    ### `QList<unique_ptr>::indexOf()` requires the item being searched for to be a `unique_ptr`.##
    ### In order to meet this requirement, `child` would have to be wrapped into a temporary     ##
    ### `unique_ptr`. This could potentially cause a double delete if the temporary `unique_ptr` ##
    ### is not explicitly released.                                                              ##
    #############################################################################################*/

    return -1;
}

/*
Constructs a new item/node and adds it to the child list. To construct the item, the function
`constructChild(args)`. If the child returned is null (nullptr), then nothing else will happen
and the function will return false. So, code to cancel an addition can be implemented there.
True will be returned if the child was constructed and added successfully, false otherwise.
*/
bool ProjectListItem::addChild(const QVariantList& args) {
    auto newChild = this->constructChild(args);
    if (newChild) {
        return false;
    }
    else {
        newChild->parentPtr = this;
        children.push_back(std::move(newChild));
        return true;
    }
}

/*
Removes the child at `index`. Before removing the child, `cleanup()` will be called on that child to
perform any side-effects required and do some cleanup. The child will only be removed if this call
returns true. So, code to cancel a removal can be implemented there. True will be returned if the
child was removed successfully, false otherwise.
*/
bool ProjectListItem::removeChild(int index) {
    if (children[index]->cleanup()) {
        return true;
    }
    else {
        return false;
    }
}



//~ProjectFile implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectFile::ProjectFile(const QFileInfo& _file) : file{_file} {}

QVariant ProjectFile::data(int role) const {
    if (role == Qt::DisplayRole)
        return QVariant{file.fileName()};
    else
        return QVariant{};
}

std::unique_ptr<ProjectListItem> ProjectFile::constructChild(const QVariantList& args) {
    return std::unique_ptr<ProjectListItem>(nullptr);
}

bool ProjectFile::cleanup() {
    auto answer = QMessageBox::question(0, "Remove file from project", QString("Are you sure you want to remove the file {0}?").arg(file.fileName()),
                                       QMessageBox::No, QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        QDir dir = file.absoluteDir();
        return dir.remove(file.fileName());
    }
    else {
        return false;
    }
}
