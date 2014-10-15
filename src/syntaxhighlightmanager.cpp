/*
Project: Lepton Editor
File: syntaxhighlightmanager.cpp
Author: Leonardo Banderali
Created: August 26, 2014
Last Modified: October 14, 2014

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

SyntaxHighlightManager::SyntaxHighlightManager(QsciScintilla* _parent) {
/* -Class constructor */

    //initialize private data members
    parent = _parent;                   //initialize parent object
    langFileLexer = new LeptonLexer();  //create the lexer which will be populated with data from a language file
    languageMenu = new QMenu("Language");
    languageActions = new QActionGroup(languageMenu);

    //get the language files' directory
    QString languagesDirPath = LeptonConfig::mainSettings.getLangsDirPath();//get path to the directory where the language files are
    QDir languagesDir(languagesDirPath);                                    //create a directory object for it

    //define the normal language actions based on the data from language files
    if ( languagesDir.exists() ) {      //only proceed if the languages directory actually exists (which it should)
        QFileInfoList languageFiles = languagesDir.entryInfoList( QStringList("*.xml"), QDir::Files ); //get a list of all the XML files (language files)

        foreach(const QFileInfo& languageFile, languageFiles) { //for every language file found
            QFile langFileObject( languageFile.absoluteFilePath() );    //create an object to read the contents of the file

            QDomDocument languageData("languageXmlData");               //create an XML DOM object to store the language data
            if (! languageData.setContent(&langFileObject) ) continue;  //get the XML data from the language file
            QDomElement languageElement = languageData.documentElement();//get root element of the XML document

            /*##############################################################
            ### I have chosen to use the following code instead of using  ##
            ###  `if (languageElement.tagName() == "language") continue;` ##
            ### because I may later decide later on to do something       ##
            ### different when a  different root element is defined.      ##
            ##############################################################*/
            if (languageElement.tagName() == "language") {              //if the file defines a programming language
                QString languageName = languageElement.attribute("name");   //get the name of the language that will be dipslayed in the language menu
                if ( languageName.isEmpty() ) continue;                     //if no 'name' attribute is defined, consider the DOM data as invalid and continue

                QAction* languageAction = new QAction(languageName, languageActions);//create a menu action for the language
                languageAction->setCheckable(true);                         //set the action to be checkable when it is clickec/selected in the language menu
                languageAction->setChecked(false);                          //explicitly set the action as being unchecked by default (note strictly required)
                languageAction->setData( languageFile.absoluteFilePath() ); //set the data of the action as the path to the language file

                if ( languageElement.hasAttribute("ext") ) {                //if a list of extensions is given, add it to the extension table for lookup
                    QStringList extList = languageElement.attribute("ext").split( QRegularExpression("\\s") );  //split the extension list at white spaces
                    ExtensionActionPair pair(extList, languageAction);      //associate the extensions with the language action
                    fileExtensionTable.append(pair);                        //add the pair to the table
                }

                languageActions->addAction(languageAction);                 //add language action to the action group
            }
        }

        //define the plain text lexer
        languageActions->addAction(new QAction(languageActions))->setSeparator(true); //%% may need to be removed (not sure what this does) %%
        plainTextAction = languageActions->addAction("Plain Text");
        plainTextAction->setCheckable(true);
        plainTextAction->setChecked(true);
        parent->setLexer(langFileLexer);
        langFileLexer->getLanguageData();

        //define list of specialized lexers
        addSpecialLanguage("CSS", new QsciLexerCSS(), "css");
        addSpecialLanguage("HTML", new QsciLexerHTML(), "html htm");
        addSpecialLanguage("Javascript", new QsciLexerJavaScript(), "js json");
        addSpecialLanguage("PHP", new QsciLexerHTML(), "php");  //the HTML lexer also serves for PHP
        addSpecialLanguage("XML", new QsciLexerXML(), "xml");
        addSpecialLanguage("YAML", new QsciLexerYAML(), "yaml");

        //add all language actions to the language menu
        languageMenu->addActions(languageActions->actions());
    }
}


SyntaxHighlightManager::~SyntaxHighlightManager() {
/* -Class destructor */

    parent = 0;
    langFileLexer->setEditor(parent);
    delete langFileLexer;
    QList<QAction* > actionList = specialLanguages.keys();
    for (int i = 0, c = actionList.count(); i < c; i++) {
        QsciLexer* lexer = specialLanguages[ actionList[i] ];
        lexer->setEditor(parent);
        delete specialLanguages[ actionList[i] ];
    }
    specialLanguages.clear();
    for (int i = 0, c = fileExtensionTable.count(); i < c; i++) {
        fileExtensionTable[i].nullActionPointer();
    }
    languageMenu->setParent(parent);
    delete languageMenu;
    actionList = languageActions->actions();
    delete languageActions;
    for (int i = 0, c = actionList.count(); i < c; i++) {
        delete actionList[i];
    }
}


QMenu* SyntaxHighlightManager::getLanguageMenu() {
/* -access function to get the language menu created from the language actions */
    return languageMenu;
}


QsciLexer* SyntaxHighlightManager::getLexerFromAction(QAction* langAction) {
/*
-return a language's syntax highlighting lexer based on the associated action
-return a 0 pointer if an error occures
*/

    QsciLexer* lexer = 0;                           //the language lexer to be returned

    if (langAction == 0){                           //if the action is null, return the plain text lexer
        lexer = langFileLexer;                              //plain text lexer should be first in the list
    }
    else {
        if (langAction == plainTextAction) {                //for plain text action, set the language
            langFileLexer->getLanguageData();
            lexer = langFileLexer;
        }
        else if ( specialLanguages.contains(langAction) ) { //if the action corresponds to one of the special, predefined languages

            //get associated special lexer to be returned
            lexer = specialLanguages[langAction];

            //set the default styling for the lexer
            lexer->setDefaultPaper( LeptonConfig::mainSettings.getValueAsColor("theme_data", "paper_color") );
            lexer->setDefaultColor( LeptonConfig::mainSettings.getValueAsColor("theme_data", "text_color") );
            lexer->setDefaultFont( LeptonConfig::mainSettings.getValueAsFont("theme_data", "font") );

            //set the styling based on the style from a "normal" lexer
            LeptonLexer l;
            l.getStyleFormat( LeptonConfig::mainSettings.getStyleFilePath("default.xml") );
            for (int i = 0; i < 128; i++) {
                lexer->setColor(l.color(i % 32), i);
                lexer->setFont(l.font(i % 32), i);
                lexer->setPaper(l.paper(i % 32), i);
            }
        }
        else {                                              //else the action must correspond to a language defined in a file
            QString filePath = langAction->data().toString();   //get the language file path
            bool ok = langFileLexer->getLanguageData(filePath); //set lexer data based on the data from the language file
            if (ok) lexer = langFileLexer;                      //if the lexer data was set successfully, assign the lexer for return
        }
    }

    return lexer;                                   //return the lexer matched with the action
}


void SyntaxHighlightManager::setLexerFromAction(QAction* langAction) {
/*  -a convenience method to set the language lexer of the parent editor based on the associated action */

    if (langAction == 0){                               //if the action is null, set the plain text lexer
        parent->setLexer(langFileLexer);                    //set the lexer
        langFileLexer->getLanguageData();                   //set the language
    }
    else {
        if ( specialLanguages.contains(langAction) ){   //if the action corresponds to one of the special, predefined languages
            parent->setLexer(specialLanguages[langAction]);     //set associated special lexer
            if (langAction == plainTextAction) {                //for plain text action, set the language
                langFileLexer->getLanguageData();
            }
        }
        else {                                          //else the action must correspond to a language defined in a file
            QString filePath = langAction->data().toString();   //get the language file path
            parent->setLexer(langFileLexer);
            langFileLexer->getLanguageData(filePath);            //set lexer data based on the data from the language file

            //if the action is not checked in the menu, check it
            if (langAction != languageActions->checkedAction() ) langAction->setChecked(true);
        }
    }
}


QsciLexer* SyntaxHighlightManager::getLexerFromSuffix(const QString& ext) {
/*  -return a language's syntax highlighting lexer based on the extension of a file */

    QsciLexer* lexer = 0;                           //the language lexer to be returned

    for (int i = 0, c = fileExtensionTable.count(); i < c; i++) {   //for every 'extension list - action' pair
        if ( fileExtensionTable[i].hasExtension(ext) ) {                //if the extension is in the list
            QAction* a = fileExtensionTable[i].getLanguageAction();         //get the corresponding language action
            lexer = getLexerFromAction(a);                                  //set the lexer to be returned
            break;                                                          //break out of the loop for return
        }
    }

    return lexer;
}


void SyntaxHighlightManager::setLexerFromSuffix(const QString& ext) {
/*  -a convenience method to set the language lexer of the parent editor based on the extension of a file */

    //parent->setLexer( getLexerFromSuffix(ext) );
    for (int i = 0, c = fileExtensionTable.count(); i < c; i++) {   //for every 'extension list - action' pair
        if ( fileExtensionTable[i].hasExtension(ext) ) {                //if the extension is in the list
            QAction* a = fileExtensionTable[i].getLanguageAction();         //get the corresponding language action
            setLexerFromAction(a);                                          //set the lexer
            break;                                                          //break out of the loop for return
        }
    }
}


QAction* SyntaxHighlightManager::getLangActionFromSuffix(const QString& ext) {
/*  -return the language action for a suffix (file extensions) */

    QAction* action = 0;                            //the language action to be returned

    for (int i = 0, c = fileExtensionTable.count(); i < c; i++) {   //for every 'extension list - action' pair
        if ( fileExtensionTable[i].hasExtension(ext) ) {                //if the extension is in the list
            action = fileExtensionTable[i].getLanguageAction();             //get the corresponding language action
            break;                                                          //break out of the loop for return
        }
    }

    return action;
}



//~private methodes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SyntaxHighlightManager::addSpecialLanguage(const QString& name, QsciLexer* lexer, const QString& extList) {
/*  -add a special language lexer to the list using its name, lexer, and file extension (suffix) list */
    QAction* langAction = languageActions->addAction(name);
    langAction->setCheckable(true);
    langAction->setChecked(false);
    specialLanguages[langAction] = lexer;
    fileExtensionTable.append( ExtensionActionPair( extList.split( QRegularExpression("\\s") ), langAction) );
}
