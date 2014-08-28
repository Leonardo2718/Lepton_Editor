/*
Project: Lepton Editor
File: syntaxhighlightmanager.cpp
Author: Leonardo Banderali
Created: August 26, 2014
Last Modified: August 27, 2014

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
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexeryaml.h>

//include Lepton files which are needed by this class
#include "leptonlexer.h"

//include Lepton files used for this class implementation
#include "generalconfig.h"



SyntaxHighlightManager::SyntaxHighlightManager(ScintillaEditor* _parent) {
/* -Class constructor */

    //initialize private data members
    parent = _parent;                   //initialize parent object

    //get the language files' directory
    QString languagesDirPath = GeneralConfig::getLangsDirPath();    //get path to the directory where the language files are
    QDir languagesDir(languagesDirPath);                            //create a directory object for it

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

        //define list of specialized lexers
        languageActions->addAction(new QAction(languageActions))->setSeparator(true); //%% may need to be removed (not sure what this does) %%
        QAction* langAction = languageActions->addAction("Plain Text");
        langAction->setCheckable(true);
        langAction->setChecked(true);
        specialLanguages[langAction] = new LeptonLexer();

        langAction = languageActions->addAction("HTML");
        langAction->setCheckable(true);
        langAction->setChecked(false);
        specialLanguages[langAction] = new QsciLexerHTML();

        langAction = languageActions->addAction("CSS");
        langAction->setCheckable(true);
        langAction->setChecked(false);
        specialLanguages[langAction] = new QsciLexerCSS();

        langAction = languageActions->addAction("XML");
        langAction->setCheckable(true);
        langAction->setChecked(false);
        specialLanguages[langAction] = new QsciLexerXML();

        langAction = languageActions->addAction("YAML");
        langAction->setCheckable(true);
        langAction->setChecked(false);
        specialLanguages[langAction] = new QsciLexerYAML();

        //add all language actions to the language menu
        languageMenu->addActions(languageActions->actions());
    }
}

SyntaxHighlightManager::~SyntaxHighlightManager() {
/* -Class destructor */
    parent = 0;
}
