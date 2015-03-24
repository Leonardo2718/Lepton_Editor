/*
Project: Lepton Editor
File: leptonproject.h
Author: Leonardo Banderali
Created: March 15, 2015
Last Modified: March 21, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The LeptonProject class represents a project being edited in Lepton.

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

#ifndef LEPTONPROJECT_H
#define LEPTONPROJECT_H

// include Qt classes
#include <QObject>
#include <QString>
#include <QDir>
#include <QVariant>
#include <QActionGroup>



/*
A general class to represent a single item in a Lepton project.
*/
class LeptonProjectItem : QObject {
        Q_OBJECT

    public:
        //constructors and destructor

        LeptonProjectItem(const QString& _name, const QString& _type, LeptonProjectItem* _parent = 0) {
            name = _name;
            type = _type;
            projectParent = _parent;
            children.clear();
        }

        virtual ~LeptonProjectItem() {
            clear();
            projectParent = 0;
        }

        // getters and setters

        virtual QString getName() const {
            return name;
        }

        virtual QString getType() const {
            return type;
        }

        QList<QAction*> getActions() {
            return contextMenuActions->actions();
        }

        // other public functions

        bool hasChildren() const {
            return !children.isEmpty();
        }

        int childCount() const {
            return children.length();
        }

        int getChildIndex(LeptonProjectItem* child) const {
            if (children.isEmpty())
                return -1;
            else
                return children.indexOf(child);
        }

        void addChild(const QString& _name, const QString& _type) {
            LeptonProjectItem* i = new LeptonProjectItem(_name, _type, this);
            children.append(i);
        }

        const LeptonProjectItem* getChild(int index) const {
            if (index < 0 || index >= children.length())
                return 0;
            else
                return children[index];
        }

        const LeptonProjectItem* getParent() const {
            return projectParent;
        }

    protected:
        QString name;                       // stores the name of the project item
        QString type;                       // stores the type of the project item
        LeptonProjectItem* projectParent;   // points to the parent item
        QList<LeptonProjectItem*> children; // points to all child items
        QActionGroup* contextMenuActions;   // stores the menu actions that can be used on the project item

        LeptonProjectItem() {   // hide default constructor from outside classes but let subclasses use it
            children.clear();
        }

        LeptonProjectItem(const LeptonProjectItem& other){}             //hide default copy constructor
        LeptonProjectItem& operator= (const LeptonProjectItem& rhs){}   //hide default assignment operator

        /* -removes all childrent from the item */
        virtual void clear() {
            for (int i = 0, l = children.length(); i < l; i++) {
                delete children.at(i);
                children[i] = 0;
            }
            children.clear();
        }
};

/*
The main Lepton project class.
*/
class LeptonProject : public LeptonProjectItem {
    public:
        // constructors and destructor
        explicit LeptonProject(const QString& projectDir, const QString& specPath = 0);
        explicit LeptonProject(const QDir& projectDir, const QString& specPath = 0);
        ~LeptonProject();

        // getters and setters
        void setName(const QString& newName);
        const QString& getSpecFilePath();   // returns path to the project's spec file

        // other public methods

        void loadSpec(const QString& filePath);
        /* -loads the project specification from a file */

        void loadProject();
        /* -load the contents of the project */

    private:

        QDir workingDirectory;      // stores the project's working directory
        QVariantMap projectSpec;    // stores the project specification
        QString specFilePath;       // stores path to the project's spec file

        void loadDir(QDir dir, QVariantMap dirSpec, QList<QVariant> parentDirTypeSpecs = QList<QVariant>(),
                     QList<QVariant> parentFileTypeSpecs = QList<QVariant>());
        /* -loads the contents of a directory that is part of the project */

        bool addItemIfMatched(const QFileInfo& fsItem, LeptonProjectItem* item, const QString& pattern, const QString& type);
        /*
            -if the file system item `fsItem`s name matches `pattern`, it is added to the project item `item`
            -returns true if the file was added, false if not
        */
};

#endif // LEPTONPROJECT_H
