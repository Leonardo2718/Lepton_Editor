/*
Project: Lepton Editor
File: projectlistitem.h
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 11, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectListItem abstract class represents an item in the project list. The project
    list is represented and stored as a tree and instances of this class are nodes of the
    three. Specializations of this class are used to implement different behavior for
    different types of items.

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

#ifndef PROJECTLISTITEM_H
#define PROJECTLISTITEM_H

// Qt classes
#include <QString>
#include <QVariant>
#include <QFileInfo>
#include <QDir>

// c++ standard libraries
#include <memory>
#include <vector>



/*
An abstract class represents an item in the project list.
*/
class ProjectListItem {
    public:
        virtual ~ProjectListItem() noexcept;

        ProjectListItem* parent() noexcept;

        int childCount() const noexcept;

        ProjectListItem* childAt(int row) noexcept;

        int indexOfChild(ProjectListItem* child) const noexcept;

        bool addChild(const QVariantList& args = QVariantList{});
        /*  Constructs a new item/node and adds it to the child list. To construct the item, the function
            `constructChild(args)`. If the child returned is null (nullptr), then nothing else will happen
            and the function will return false. So, code to cancel an addition can be implemented there.
            True will be returned if the child was constructed and added successfully, false otherwise.
        */

        bool removeChild(int index);
        /*  Removes the child at `index`. Before removing the child, `cleanup()` will be called on that child to
            perform any side-effects required and do some cleanup. The child will only be removed if this call
            returns true. So, code to cancel a removal can be implemented there. True will be returned if the
            child was removed successfully, false otherwise.
        */

        virtual QVariant data(int role = Qt::DisplayRole) const = 0;
        /*  Returns the data stored in the node that corresponds to a given `role`.
            Sub-classes must reimplement this function.
        */

    protected:
        virtual std::unique_ptr<ProjectListItem> constructChild(const QVariantList& args = QVariantList{}) = 0;
        /*  Constructs and returns a new child item from `args` without adding it to its child list.
            `args` will contain the arguments used to construct the new item. Sub-classes must reimplement
            this function. All side effects of adding a child must be implemented here.
        */

        virtual bool cleanup() = 0;
        /*  Cleans side-effects of item; returns true if successful, false otherwise. Sub-classes must reimplement
            this function. All the side effects of removing an item must be implemented here. Note, this
            function should not attempt to delete/free any nodes!
        */


    private:
        std::vector<std::unique_ptr<ProjectListItem>> children;
        ProjectListItem* parentPtr = nullptr;
};



//~ProjectListItem specializations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
A class representing a project item that is also a file system item
*/
class ProjecFileSystemItem: public ProjectListItem {
    public:
        virtual QString path() const noexcept = 0;
        /*  returns file system path to the item */
};

/*
A class representing a file in a project.
*/
class ProjectFile: public ProjecFileSystemItem {
    public:
        ProjectFile(const QFileInfo& _file);

        QVariant data(int role = Qt::DisplayRole) const;

        QString path() const noexcept;

    protected:
        std::unique_ptr<ProjectListItem> constructChild(const QVariantList& args = QVariantList{});

        bool cleanup();

    private:
        QFileInfo file;
};

/*
A class representing a directory in a project.
*/
class ProjectDirectory: public ProjecFileSystemItem {
    public:
        ProjectDirectory(const QDir& _dir);

        QVariant data(int role = Qt::DisplayRole) const;

        QString path() const noexcept;

    protected:
        std::unique_ptr<ProjectListItem> constructChild(const QVariantList& args = QVariantList{});

        bool cleanup();

    private:
        QDir dir;
};

/*
A class representing a project in the list.
*/
class Project: public ProjecFileSystemItem {
    public:
        Project(const QDir& _projectDir);

        QVariant data(int role = Qt::DisplayRole) const;

        QString path() const noexcept;

    protected:
        std::unique_ptr<ProjectListItem> constructChild(const QVariantList& args = QVariantList{});

        bool cleanup();

    private:
        QDir projectDir;
};

/*
A class representing the root of a project list.
*/
class ProjectListRoot: public ProjectListItem {
    public:
        ProjectListRoot();

        QVariant data(int role = Qt::DisplayRole) const;

    protected:
        std::unique_ptr<ProjectListItem> constructChild(const QVariantList& args = QVariantList{});

        bool cleanup();
};

#endif // PROJECTLISTITEM_H

