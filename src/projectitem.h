/*
Project: Lepton Editor
File: projectitem.h
Author: Leonardo Banderali
Created: June 9, 2014
Last Modified: June 15, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The 'ProjectItem' class is intended to represent one item in a project model.  This may be
    a directory or a file which is part of a project.

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

#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include <QFileInfo>
#include <QString>
#include <QList>
#include <QVariant>
#include <QIcon>

class ProjectItem {
    public:
        explicit ProjectItem(const QList< QVariant >& data, ProjectItem* parent = 0);
        ProjectItem(const QString& itemPath, ProjectItem *parent);
        ~ProjectItem();

        void appendChild(ProjectItem* newItem);
        void appendChild(const QString itemPath);
        /* -adds a child item to this item */

        void removeChild(ProjectItem* child);
        /* -removes a child form this item */

        ProjectItem* child(int index);
        /* -returns child item at 'index' */

        QVariant getDisplayData(int column) const;
        /* -returns data to be displayed in a specified column */

        QIcon getDecorationIcon() const;
        /* -returns icon to be displayed next to the item display data in a view */

        ProjectItem* parent();
        /* -returns pointer to parent item (null if this item is root) */

        QString getPath() const;
        /* -returns path to this item */

        int childCount() const;
        /* -returns number of child items */

        int columnCount() const;
        /* -returns number of data elements */

        int dataElementCount() const;
        /* -returns number of data elements (same as 'columnCount()' ) */

        int currentIndex() const;
        /* -returns the index of this item in its parent ('0' if this item is root) */

        int currentRow() const;
        /* -returns the row (index) of this item (same as 'currentIndex()') */

        bool isFile() const;
        /* -returns whether this item is a file */

        bool isDirectory() const;
        /* -returns whether this item is a directory */

    protected:
        QList< QVariant > itemDisplayData;  //data for this item
        QList< ProjectItem* > children;     //list of all child items

    private:
        ProjectItem* parentItem;    //pointer to parent item (null if this is root item)
        QFileInfo item;             //the item which is being represented

        void setParent(ProjectItem* newParent);
        /* -set new parent item of this item */
};

#endif // PROJECTITEM_H
