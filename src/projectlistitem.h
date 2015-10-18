/*
Project: Lepton Editor
File: projectlistitem.h
Author: Leonardo Banderali
Created: October 10, 2015
Last Modified: October 18, 2015

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



// forward declarations
class ProjectItemAction;



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

        //bool addChild(const QVariantList& args = QVariantList{});
        /*  Constructs a new item/node and adds it to the child list. To construct the item, the function
            `constructChild(args)`. If the child returned is null (nullptr), then nothing else will happen
            and the function will return false. So, code to cancel an addition can be implemented there.
            True will be returned if the child was constructed and added successfully, false otherwise.
        */

        void addChild(std::unique_ptr<ProjectListItem> newChild);

        //bool removeChild(int index);
        /*  Removes the child at `index`. Before removing the child, `cleanup()` will be called on that child to
            perform any side-effects required and do some cleanup. The child will only be removed if this call
            returns true. So, code to cancel a removal can be implemented there. True will be returned if the
            child was removed successfully, false otherwise.
        */

        std::unique_ptr<ProjectListItem> removeChild(ProjectListItem* child);

        virtual QVariant data(int role = Qt::DisplayRole) const = 0;
        /*  Returns the data stored in the node that corresponds to a given `role`.
            Sub-classes must reimplement this function.
        */

        virtual QList<ProjectItemAction*> contextMenuActions() const;
        /*  Returns the actions for the context menu to be displayed when this item is right-clicked in the
            project manager. Any action within this group must store as its data a pointer to the item it
            belongs to.
        */

        virtual QList<ProjectItemAction*> newChildActions() const;
        /*  list of all actions that, when triggered, will cause a new child node to be created */

        virtual QList<ProjectItemAction*> removeActions() const;
        /*  list of all actions that, when triggered, will cause the node to be removed */

        virtual QList<ProjectItemAction*> changeDataActions() const;
        /*  list of all actions that, when triggered, will cause the data of the node to be changed */

        virtual bool handleNewChildAction(ProjectItemAction* action);
        /*  handles the creation of a new child */

        virtual bool handleRemoveAction(ProjectItemAction* action);
        /*  handles the removal of this item */

        virtual bool handleChangeDataAction(ProjectItemAction* action);
        /*  handles changing the data of this item */

        virtual QList<ProjectListItem*> loadChildren() = 0;
        /*  loads all children based on what this current item is */

    private:
        std::vector<std::unique_ptr<ProjectListItem>> children;
        ProjectListItem* parentPtr = nullptr;
};

class ProjectItemAction : public QAction {
    public:
        ProjectItemAction(const QString& _text, ProjectListItem* _item);

        ProjectListItem* item() const noexcept;

    private:
        ProjectListItem* projectItem;
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

        QList<ProjectItemAction*> contextMenuActions() const;

        //QList<ProjectItemAction*> newChildActions() const;

        QList<ProjectItemAction*> removeActions() const;

        //QList<ProjectItemAction*> changeDataActions() const;

        //bool handleNewChildAction(ProjectItemAction* newChildAction);

        bool handleRemoveAction(ProjectItemAction* action);

        //bool handleChangeDataAction(ProjectItemAction* changeDataAction);

    private:
        QDir projectDir;            // directory containing the project
        ProjectItemAction* closeAction;       // action to close this project
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

        ProjectListItem* loadProject(const QString& projectPath);
};

#endif // PROJECTLISTITEM_H

