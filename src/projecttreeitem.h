/*
Project: Lepton Editor
File: projecttreeitem.h
Author: Leonardo Banderali
Created: April 5, 2015
Last Modified: May 7, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTreeItem class represents a single item (node) in the Lepton projects tree structure.

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

#ifndef PROJECTTREEITEM_H
#define PROJECTTREEITEM_H

// include Qt classes
#include <QObject>
#include <QList>
#include <QVariant>
#include <QMap>
#include <QAction>
#include <QActionGroup>

class ProjectTreeItem : public QObject {
        Q_OBJECT

    public:
        // constructors and destructor
        explicit ProjectTreeItem(const QVariantMap& _data, ProjectTreeItem* _parent);
        virtual ~ProjectTreeItem();

        // getters and setters
        const ProjectTreeItem* getParent() const;
        virtual const ProjectTreeItem* getChild(int index) const;       // returns pointer to the child with a given index
        virtual int getChildIndex(const ProjectTreeItem* child) const;  // returns internal index of a child
        const QVariantMap& getData() const;
        QVariant getDataItem(const QString& key) const;                 // returns the data item associated with a given key

        // other public functions

        virtual bool hasChildren() const;

        virtual int childCount() const;

        virtual const ProjectTreeItem* addChild(const QVariantMap& _data);

        virtual bool removeChild(ProjectTreeItem* child);        // removes a specific child

        void addContextMenuAction(QAction* a);
        QActionGroup* getContextMenuActions();

        virtual void load();
        /*  -loads this item by creating its children */

        virtual void reload();
        /*  -reloads this item (currently does the same as `load()`) */

    signals:
        void removingItem(const ProjectTreeItem*);
        void itemRemoved();
        void changingItem(const ProjectTreeItem*);
        void itemChanged();

    protected:
        QVariantMap data;                                   // data stored by the item

        void clear();                                       // removes all children

        void loadAsDir();
        /*  -loads the contents of a directory as part of this item */

        void addContextActionsFor(ProjectTreeItem* item, const QVariantMap contextSpec);
        /*  -sets the context menu actions for `item` based on it's type */

        bool itemNameMatches(const QString& itemName, const QString& pattern);
        /*  -return true if `itemName` matches the pattern */

    private:
        ProjectTreeItem* parent;            // pointer to parent tree item
        QList<ProjectTreeItem*> children;   // pointers to all child items
        QActionGroup* contextMenuActions;   // actions for the item's context menu
};

#endif // PROJECTTREEITEM_H
