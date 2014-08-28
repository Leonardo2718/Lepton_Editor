/*
Project: Lepton Editor
File: syntaxhighlightmanager.h
Author: Leonardo Banderali
Created: August 26, 2014
Last Modified: August 27, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the definition for the SyntaxHighlightManager class.  It is used
    to manage the syntax highlighting files as well as to provide a menu for selecting
    or changing the language used for highlighting.  This class is intended to provide
    an interface for managing the language lexers used in the ScintillaEditor class.

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



#ifndef SYNTAXHIGHLIGHTMANAGER_H
#define SYNTAXHIGHLIGHTMANAGER_H

//include Qt classes
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QList>
#include <QStringList>
#include <QHash>

//include QScintilla classes
#include <Qsci/qscilexer.h>

//include Lepton files which are needed by this class
#include "scintillaeditor.h"



class SyntaxHighlightManager {
/* -A class that provides an interface for managing the language lexers used in the ScintillaEditor class. */

    public:
        explicit SyntaxHighlightManager(ScintillaEditor* _parent);
        /*
            -Class constructor
            -initialize the parent object which points to the ScintillaEditor class which uses this manager.
        */

        virtual ~SyntaxHighlightManager();
        /*
            -Class destructor
            -perform garbage collections (free allocated memory and null all pointers)
        */

        class ExtensionActionPair {
        /* -A class that stores a list of file extensions and associates it to a language action */
            public:
                explicit ExtensionActionPair(QStringList _extensionList, QAction* _action) {
                /* -the class constructor initializes the data members */
                    extensionList = _extensionList;
                    action = _action;
                }

                bool hasExtension(const QString& extension) const {
                /* -return true if the extension is in the list, false otherwise */
                    if ( extensionList.contains(extension) ) return true;
                    else return false;
                }

                QAction* getLanguageAction() const {
                /* -return the language action */
                    return action;
                }

                QAction* getActionIfHasExt(const QString& extension) const {
                /* -return the language action if the extension passed is in the list, else return a 0 pointer */
                    if ( extensionList.contains(extension) ) return action;
                    else return 0;
                }

            private:
                QStringList extensionList;
                QAction* action;
        };

    private:
        ScintillaEditor* parent;                        //pointer the editing class which uses this manager
        QActionGroup* languageActions;                  //a group of actions to use for selecting a syntax highlighting language
        QMenu* languageMenu;                            //a menu to select a language
        QList<ExtensionActionPair> fileExtensionTable;  //a table to match file extensions with an action to select a language
        QHash<QAction*, QsciLexer*> specialLanguages;   //a list of specialized languages/lexers which do not require user definition
};

#endif // SYNTAXHIGHLIGHTMANAGER_H
