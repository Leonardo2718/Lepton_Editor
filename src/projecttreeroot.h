/*
Project: Lepton Editor
File: projecttree.h
Author: Leonardo Banderali
Created: April 5, 2015
Last Modified: May 7, 2015

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

class ProjectTreeRoot : public ProjectTreeItem {
    Q_OBJECT

    public:
        // constructors and destructor
        explicit ProjectTreeRoot();
        ~ProjectTreeRoot();

        virtual const ProjectTreeItem* getChild(int index) const;           // returns pointer to the child with a given index
        virtual int getChildIndex(const ProjectTreeItem* child) const;      // returns internal index of a child
        virtual bool hasChildren() const;
        virtual int childCount() const;
        bool removeChild(ProjectTreeItem* child);                          // must be overloaded to avoid calling ProjectTreeItem::removedChild(ProjectTreeItem*)

        // other public functions
        void createNewProject();    // opens dialog to create a new project
        void openProject();         // opens dialog to open an already existing project
        void openProject(const QString& projectPath, const QString& specPath = 0);   // opens the project specified
        void closeProject(ProjectTreeItem* project);    // closes a project if it exists, other wise does nothing

    private:
        QList<LeptonProject*> children;

        virtual const ProjectTreeItem* addChild(const QString& dirName, const QString& specPath = 0);
};

#endif // PROJECTTREE_H
