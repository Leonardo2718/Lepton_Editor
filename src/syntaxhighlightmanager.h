/*
Project: Lepton Editor
File: syntaxhighlightmanager.h
Author: Leonardo Banderali
Created: August 26, 2014
Last Modified: August 31, 2014

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
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>

//include Lepton files which are needed by this class
#include "leptonlexer.h"



class SyntaxHighlightManager {
/* -A class that provides an interface for managing the language lexers used in the ScintillaEditor class. */

    public:
        explicit SyntaxHighlightManager(QsciScintilla *_parent);
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

                void nullActionPointer() {
                /*  -nulls the pointer to the language action*/
                    action = 0;
                }

            private:
                QStringList extensionList;
                QAction* action;
        };

        QMenu* getLanguageMenu();
        /*  -access function to get the language menu created from the language actions */

        QsciLexer* getLexerFromAction(QAction* langAction = 0);
        /*
            -return a language's syntax highlighting lexer based on the associated action
            -return a 0 pointer if an error occures
        */

        void setLexerFromAction(QAction* langAction = 0);
        /*  -a convenience method to set the language lexer of the parent editor based on the associated action */

        QsciLexer* getLexerFromSuffix(const QString& ext);
        /*  -return a language's syntax highlighting lexer based on the extension of a file */

        void setLexerFromSuffix(const QString& ext);
        /*  -a convenience method to set the language lexer of the parent editor based on the extension of a file */

    private:
        QsciScintilla* parent;                          //pointer the editing class which uses this manager
        QActionGroup* languageActions;                  //a group of actions to use for selecting a syntax highlighting language
        QMenu* languageMenu;                            //a menu to select a language
        QAction* plainTextAction;                       //action to set highlighting for plain text document
        QList<ExtensionActionPair> fileExtensionTable;  //a table to match file extensions with an action to select a language
        QHash<QAction*, QsciLexer*> specialLanguages;   //a list of specialized languages/lexers which do not require user definition
        LeptonLexer* langFileLexer;                     //the lexer used for languages defined in files
};

#endif // SYNTAXHIGHLIGHTMANAGER_H
