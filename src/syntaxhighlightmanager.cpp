/*
Project: Lepton Editor
File: syntaxhighlightmanager.cpp
Author: Leonardo Banderali
Created: August 26, 2014
Last Modified: December 28, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation for the SyntaxHighlightManager class.  It is used
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



//include this file's header
#include "syntaxhighlightmanager.h"

//include Qt classes
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDomDocument>
#include <QDomElement>
#include <QRegularExpression>

//include QScintilla classes
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexeryaml.h>

//include Lepton files used for this class implementation
#include "leptonconfig.h"



//~private methodes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SyntaxHighlightManager::SyntaxHighlightManager(QsciScintilla* _parent) : QObject((QObject*)_parent) {
/* -Class constructor */

    //initialize private data members
    parent = _parent;                   //initialize parent object to the editor
    lexer = new LeptonLexer();
    languageMenu = new QMenu("Language");
    languageActions = new QActionGroup(languageMenu);
    parent->setLexer(lexer);
    lexer->loadLanguage();

    //get the language files' directory
    QString languagesDirPath = LeptonConfig::mainSettings.getLangsDirPath();//get path to the directory where the language files are
    QDir languagesDir(languagesDirPath);                                    //create a directory object for it

    QList<QAction*> specialActionList;

    //define plain text action
    plainTextAction = languageActions->addAction("Plain Text");
    plainTextAction->setCheckable(true);
    plainTextAction->setChecked(true);
    plainTextAction->setData( QString() );
    specialActionList.append(plainTextAction);

    //add special actions to the menu
    languageMenu->addActions(specialActionList);

    //if the languages directory exists (it should), define language selection actions
    if ( languagesDir.exists() ) {
        getLanguages(languagesDir, languageMenu);
    }

    connect(languageActions, SIGNAL(triggered(QAction*)), this, SLOT(languageSelected(QAction*)));
}


SyntaxHighlightManager::~SyntaxHighlightManager() {
/* -Class destructor */
    parent = 0;
    filemaskList.clear();

    delete lexer;
    delete languageMenu;
}


QMenu* SyntaxHighlightManager::getLanguageMenu() {
/* -access function to get the language menu created from the language actions */
    return languageMenu;
}

QString SyntaxHighlightManager::setLexerForFile(const QString& fileName) {
/*
-sets an appropriate lexer for `fileName` based on filemasks
-returns the name of the language selected
*/
    QAction* a = plainTextAction;

    for (int i = 0, c = filemaskList.count(); i < c; i++) {
        QRegularExpressionMatch m = filemaskList.at(i).filemask.match(fileName);
        if ( m.hasMatch() ) {
            a = filemaskList.at(i).langAction;
            break;
        }
    }

    lexer->loadLanguage( a->data().toString() );
    a->setChecked(true);
    emit changedLexerLanguage( a->text() );
    return a->text();

    /*###############################################################################
    ### Note that no re-coloring needs to be done since `a->setChecked(true)` will ##
    ### emit a signal that will cause this to happen anyways.                      ##
    ###############################################################################*/
}



//~private methodes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SyntaxHighlightManager::getLanguages(const QDir& langDir, QMenu* langMenu) {
/*  -adds a language selection action to `langMenu` for each language defined by a file in `langDir` */
    if ( ! langDir.exists() || ! langDir.isReadable() ) return;

    QFileInfoList dirEntries = langDir.entryInfoList( QStringList("*.xml"), QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable, QDir::Name | QDir::LocaleAware);
    QList< QAction* > actionList;   //list of actions to be added to `langMenu`

    foreach (const QFileInfo& entry, dirEntries) {
        if ( entry.isDir() ) {
            //if the entry is a subdirectory, create a new menu and put into it
            //  the actions for each language defined in that directory
            QMenu* newSubMenu = langMenu->addMenu( entry.fileName() );
            QDir subDir = entry.absoluteDir();
            subDir.cd( entry.fileName() );
            getLanguages(subDir, newSubMenu);
        }
        else {
            QFile langFile( entry.absoluteFilePath() );

            QDomDocument langData("languageData");              //an XML DOM object to store the language data
            if (! langData.setContent(&langFile) ) continue;
            QDomElement docElement = langData.documentElement();

            if (docElement.tagName() == "language") {           //if the file defines a programming language
                QString langName = docElement.attribute("name");    //get the name of the language that will be dipslayed in the language menu
                if ( langName.isEmpty() ) continue;                 //if no 'name' attribute is defined, then the data is not valid

                //create a menu action for the language
                QAction* langAction = languageActions->addAction(langName);
                actionList.append(langAction);

                //setup the new action
                langAction->setCheckable(true);
                langAction->setChecked(false);
                langAction->setData( entry.absoluteFilePath() );//set the data of the action as the path to the current language file

                if ( docElement.hasAttribute("filemask") && !docElement.attribute("filemask").isEmpty() ) { //if the language has a valid filemask
                    FilemaskActionPair newPair;
                    newPair.filemask.setPattern( docElement.attribute("filemask") );
                    if ( newPair.filemask.isValid() ) {
                        newPair.langAction = langAction;
                        filemaskList.append(newPair);
                    }
                }

            }
        }
    }

    langMenu->addActions(actionList);
}



//~private slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SyntaxHighlightManager::languageSelected(QAction* langAction) {
/*  -responds to 'langAction' being clicked in the language menu and emits a signal to change the lexer language */
    lexer->loadLanguage( langAction->data().toString() );
    parent->recolor();
    emit changedLexerLanguage( langAction->text() );
}
