/*
Project: Lepton Editor
File: leptonproject.h
Author: Leonardo Banderali
Created: March 15, 2015
Last Modified: March 29, 2015

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
#include <QDir>
#include <QVariant>
#include <QAbstractItemModel>

// include other project headers
#include "leptonprojectitem.h"

/*
The main Lepton project class.
*/
class LeptonProject : public LeptonProjectItem {

    public:
        // constructors and destructor
        explicit LeptonProject(QAbstractItemModel* _qModel, const QString& projectDir, const QString& specPath = 0);
        //explicit LeptonProject(QAbstractItemModel* _qModel, const QDir& projectDir, const QString& specPath = 0);
        ~LeptonProject();

        // getters and setters
        void setName(const QString& newName);
        const QString& getSpecFilePath();   // returns path to the project's spec file
        virtual QList<QAction*> getActions();

        // other public methods

        void loadSpec(const QString& filePath);
        /* -loads the project specification from a file */

    public slots:
        void loadItem();
        /*  load the contents of the project */

        virtual void contextMenuActionTriggered(QAction* actionTriggered);

    private:

        QDir workingDirectory;      // stores the project's working directory
        QVariantMap projectSpec;    // stores the project specification
        QString specFilePath;       // stores path to the project's spec file
        QAbstractItemModel* qModel; // points the project tree model used to display projects

        void loadDir(LeptonProjectItem* rootItem, QDir dir, const QVariantMap& dirSpec, const QList<QVariant>& parentDirTypeSpecs = QList<QVariant>(),
                     const QList<QVariant>& parentFileTypeSpecs = QList<QVariant>());
        /* -loads the contents of a directory that is part of the project */

        bool itemNameMatches(const QString& itemName, const QString& pattern);
        /*  -return true if `itemName` matches the pattern */

        void addContextActionsFor(LeptonProjectItem* item, const QVariantMap contextSpec);
        /*  -sets the context menu actions for `item` based on it's type */
};

#endif // LEPTONPROJECT_H
