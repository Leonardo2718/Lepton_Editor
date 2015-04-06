/*
Project: Lepton Editor
File: projecttree.h
Author: Leonardo Banderali
Created: April 5, 2015
Last Modified: April 5, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTree class is represents the root item for the projects tree.  All direct
    children of this root class are actual Lepton projects.  This class is used to facilitate
    project management.

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

#ifndef PROJECTTREE_H
#define PROJECTTREE_H

// include Qt classes
#include <QList>

// include other project headers
#include "projecttreeitem.h"
#include "leptonproject.h"

class ProjectTree : public ProjectTreeItem {
    Q_OBJECT

    public:
        // constructors and destructor
        explicit ProjectTree();
        ~ProjectTree();

        virtual const ProjectTreeItem* getChild(int index) const;           // returns pointer to the child with a given index
        virtual int getChildIndex(const ProjectTreeItem* child) const;      // returns internal index of a child
        virtual const ProjectTreeItem* addChild();
        virtual bool hasChildren() const;
        virtual int childCount() const;

    private:
        QList<LeptonProject*> children;
};

#endif // PROJECTTREE_H
