/*
Project: Lepton Editor
File: leptonprojectitem.h
Author: Leonardo Banderali
Created: March 23, 2015
Last Modified: March 24, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The LeptonProjectItem class represents a single project item in any Lepton project.

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

#ifndef LEPTONPROJECTITEM_H
#define LEPTONPROJECTITEM_H

// include Qt classes
#include <QObject>
#include <QAction>
#include <QActionGroup>


/*
A general class to represent a single item in a Lepton project.
*/
class LeptonProjectItem : public QObject {
        Q_OBJECT

    public:
        //constructors and destructor

        LeptonProjectItem(const QString& _name, const QString& _type, LeptonProjectItem* _parent = 0);

        virtual ~LeptonProjectItem();

        // getters and setters
        virtual QString getName() const;
        virtual QString getType() const;
        const LeptonProjectItem* getParent() const;
        virtual QList<QAction*> getActions();

        // other public functions

        bool hasChildren() const;

        int childCount() const;

        int getChildIndex(LeptonProjectItem* child) const;

        const LeptonProjectItem* addChild(const QString& _name, const QString& _type);

        const LeptonProjectItem* getChild(int index) const;

        void addAction(QAction* a);

    public slots:
        virtual void contextMenuActionTriggered(QAction* actionTriggered);

    protected:
        QString name;                       // stores the name of the project item
        QString type;                       // stores the type of the project item
        LeptonProjectItem* projectParent;   // points to the parent item
        QList<LeptonProjectItem*> children; // points to all child items
        QActionGroup* contextMenuActions;   // stores the menu actions that can be used on the project item

        LeptonProjectItem();    // hide default constructor from outside classes but let subclasses use it

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

#endif // LEPTONPROJECTITEM_H
