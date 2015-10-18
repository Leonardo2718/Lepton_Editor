/*
Project: Lepton Editor
File: projectlistitem.h
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 17, 2015

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
#include <QObject>
#include <QString>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QAction>

// c++ standard libraries
#include <memory>
#include <vector>



/*
An abstract class represents an item in the project list.
*/
class ProjectListItem: public QObject {
    Q_OBJECT

    public:
        ProjectListItem();

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

        void addChild(std::unique_ptr<ProjectListItem> newChild);

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

        virtual QList<QAction*> contextMenuActions() const;
        /*  Returns the actions for the context menu to be displayed when this item is right-clicked in the
            project manager. Any action within this group must store as its data a pointer to the item it
            belongs to.
        */

        virtual QList<QAction*> newChildActions() const;
        /*  list of all actions that, when triggered, will cause a new child node to be created */

        virtual QList<QAction*> removeActions() const;
        /*  list of all actions that, when triggered, will cause the node to be removed */

        virtual QList<QAction*> changeDataActions() const;
        /*  list of all actions that, when triggered, will cause the data of the node to be changed */

        virtual bool handleNewChildAction(QAction* action);
        /*  handles the creation of a new child */

        virtual bool handleRemoveAction(QAction* action);
        /*  handles the removal of this item */

        virtual bool handleChangeDataAction(QAction* action);
        /*  handles changing the data of this item */

        virtual QList<ProjectListItem*> loadChildren() = 0;
        /*  loads all children based on what this current item is */

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

        QList<ProjectListItem*> loadChildren() override;

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

        QList<ProjectListItem*> loadChildren();

    protected:
        std::unique_ptr<ProjectListItem> constructChild(const QVariantList& args = QVariantList{});

        bool cleanup();

    private:
        QDir dir;
};

/*
A class representing a project in the list.
*/
class Project: public ProjectDirectory {
    Q_OBJECT
    public:
        Project(const QDir& _projectDir);

        ~Project();

        QVariant data(int role = Qt::DisplayRole) const;

        QString path() const noexcept;

        QList<QAction*> contextMenuActions() const;

        //QList<QAction*> newChildActions() const;

        QList<QAction*> removeActions() const;

        //QList<QAction*> changeDataActions() const;

        //bool handleNewChildAction(QAction* newChildAction);

        bool handleRemoveAction(QAction* action);

        //bool handleChangeDataAction(QAction* changeDataAction);

    protected:
        bool cleanup();

    private:
        QDir projectDir;            // directory containing the project
        QAction* closeAction;       // action to close this project
        //QList<QAction*> menuActions;// list of context menu actions

    private slots:
        void handleCloseProject(bool actionChecked);
        /*  handles response to a close action from the context menu */
};

/*
A class representing the root of a project list.
*/
class ProjectListRoot: public ProjectListItem {
    public:
        ProjectListRoot();

        QVariant data(int role = Qt::DisplayRole) const;

        QList<ProjectListItem*> loadChildren() override;

        QList<ProjectListItem*> loadProjects(const QList<QString>& projectDirs);

    protected:
        std::unique_ptr<ProjectListItem> constructChild(const QVariantList& args = QVariantList{});

        bool cleanup();
};

#endif // PROJECTLISTITEM_H

