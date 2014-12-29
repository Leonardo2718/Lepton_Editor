/*
Project: Lepton Editor
File: syntaxhighlightmanager.h
Author: Leonardo Banderali
Created: August 26, 2014
Last Modified: December 28, 2014

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
#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QList>
#include <QStringList>
#include <QDir>

//include QScintilla classes
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>

//include Lepton files which are needed by this class
#include "leptonlexer.h"



class SyntaxHighlightManager : public QObject{
/* -A class that provides an interface for managing the language lexers used in the ScintillaEditor class. */

    Q_OBJECT

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

        QMenu* getLanguageMenu();
        /*  -access function to get the language menu created from the language actions */

        QString setLexerForFile(const QString& fileName);
        /*
            -sets an appropriate lexer for `fileName` based on filemasks
            -returns the name of the language selected
        */

    signals:
        void changedLexerLanguage(const QString& langName);
        /*  -a signal emited when the language grammer of the lexer is changed */

    private:

        struct FilemaskActionPair {
                QRegularExpression filemask;
                QAction* langAction;
        };

        QsciScintilla* parent;                  //pointer the editing class which uses this manager
        QActionGroup* languageActions;          //a group of actions to use for selecting a syntax highlighting language
        QMenu* languageMenu;                    //a menu to select a language
        QAction* plainTextAction;               //action to set highlighting for plain text document
        LeptonLexer* lexer;                     //the lexer used for languages defined in files
        QList<FilemaskActionPair> filemaskList; //a table to match file names to a language action

        void addSpecialLanguage(QList<QAction*>& aList, const QString& name, QsciLexer* lexer, const QString& extList);
        /*  -add a special language lexer to the list using its name, lexer, and file extension (suffix) list */

        void getLanguages(const QDir& langDir, QMenu* langMenu);
        /*  -adds a language selection action to `langMenu` for each language defined by a file in `langDir` */

    private slots:
        void languageSelected(QAction* langAction);
        /*  -responds to 'langAction' being clicked in the language menu and emits a signal to change the lexer language */
};

#endif // SYNTAXHIGHLIGHTMANAGER_H
