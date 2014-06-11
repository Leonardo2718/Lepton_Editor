/*
Project: Lepton Editor
File: scintillaeditor.h
Author: Leonardo Banderali
Created: May 5, 2014
Last Modified: June 10, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of a subclass of QsciScintilla.  This class will be used as
    editing environment instead of the default QPlainTextEdit class.

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

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QDir>
#include <QList>
#include <QDomDocument>
#include <QMessageBox>
#include <Qsci/qscilexercpp.h>

#include "scintillaeditor.h"
#include "generalconfig.h"



//~public method implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ScintillaEditor::ScintillaEditor(QWidget* parent) : QsciScintilla(parent) {
/* -setup editor and configurations */
    //set and display line numbers; margin '1' is the default line number maring
    setMarginWidth(1, 55);
    setMarginLineNumbers(1, true);

    //apply lexer
    lexer = new LeptonLexer(this);
    setLexer(lexer);

    //language menu to select the language for syntax highlighting
    languageMenu = new QMenu("Language", this);
    languageGroup = new QActionGroup(languageMenu);
    langFileFromAction = new QHash<QAction*, QString>;

    //add action to select 'plain text' mode
    QAction* lang = new QAction("Plain Text", languageMenu);
    lang->setCheckable(true);
    languageGroup->addAction(lang);
    langFileFromAction->insert(lang, "" );

    //add an action for each language file
    QDir langsDir( GeneralConfig::getLangsDirPath() );
    langsDir.setNameFilters( QStringList("*.xml") );
    QStringList langsFileList = langsDir.entryList();   //get list of file paths

    //for each file in the path list, parse the file to get the language name and create an action for the language
    foreach (const QString& langFileName, langsFileList) {
        QFile langFile( langsDir.absoluteFilePath(langFileName) );
        QDomDocument langDef("language_def");
        if ( ! langDef.setContent(&langFile) ) continue;
        QString langName = langDef.documentElement().attribute("name"); //get the language name
        if ( langName.isEmpty() ) continue;
        lang = new QAction(langName, languageMenu); //create language action
        lang->setCheckable(true);
        languageGroup->addAction(lang);
        langFileFromAction->insert(lang, langsDir.absoluteFilePath(langFileName) );
    }
    languageMenu->addActions( languageGroup->actions() );
    languageMenu->setDefaultAction( langFileFromAction->key("Plain Text") );

    //connect signals to slots
    connect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(setLanguage(QAction*)) );

    //set editor properties/settings
    setAutoIndent(true);
    setTabWidth(4);
    setBraceMatching(QsciScintilla::StrictBraceMatch);
    setMarginsBackgroundColor( GeneralConfig::getMarginsBackground() );
    setMarginsForegroundColor( GeneralConfig::getMarginsForeground() );

    /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //$ Stub code used to test Scintilla features                          $$
    //$                                                                    $$

    //$                                                                    $$
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
}

ScintillaEditor::~ScintillaEditor() {
/* clean up and delete allocated memory */
    setLexer(); //clear lexer use before deleting instance
    disconnect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(setLanguage(QAction*)) );

    //get all lagnuage actions
    QList< QAction* > actionList = langFileFromAction->keys();

    //remove each action from 'languageGroup' and delete the action
    for ( int i = 0, len = actionList.length(); i < len; i++ ) {
        languageGroup->removeAction( actionList[i] );
        delete actionList[i];
    }

    //clear the action lists
    langFileFromAction->clear();
    languageMenu->clear();

    //delete allocated memory
    delete langFileFromAction;
    delete languageGroup;
    delete languageMenu;
    delete lexer;
}

void ScintillaEditor::writeToFile(const QString& filePath, bool changeModify) {
/*
-writes edited text to a file
-parameters:
    filePath: bath to file
    changeModify: if true, call setModified() to change the modified state, otherwise do nothing
*/
    QFile file(filePath);

    if ( !file.open(QIODevice::ReadWrite | QIODevice:: WriteOnly) ) {
        QMessageBox::warning(this, tr("Lepton Error"), tr("Cannot write to file %1:\n%2.").arg(filePath).arg(file.errorString()));
        return;
    }

    //resize file to fit edited text
    if ( !file.resize( this->length() ) ) {
        QMessageBox::warning(this, tr("Lepton Error"), tr("Cannot write to file %1:\n%2.").arg(filePath).arg(file.errorString()));
        return;
    }
    file.write( this->text().toUtf8() );    //write edited text
    file.close();

    if (changeModify) setModified(false);
}

void ScintillaEditor::loadFile(const QString& filePath) {
/* -load contents of a file to be edited */
    QFile file(filePath);

    if ( !file.open(QIODevice::ReadWrite) ) {
        QMessageBox::warning(this, tr("Lepton Error"), tr("Cannot open file %1:\n%2.").arg(filePath).arg(file.errorString()));
        return;
    }

    //set the text in the editor
    this->setText( file.readAll() );
    file.close();

    //save the new file path
    openFile.setFile(filePath);

    setModified(false);
}

QString ScintillaEditor::getOpenFilePath() {
/* -get the path to the file currently being edited */
    return openFile.absoluteFilePath();
}

QString ScintillaEditor::getOpenFileName() {
/* -get the name of the file currently being edited */
    return openFile.fileName();
}

bool ScintillaEditor::wasFileSaved() {
/* returns wheater changes to the open file have been saved/writen */
    return !( isModified() );
}

QMenu* ScintillaEditor::getLanguageMenu() {
/* -returns the language selection menu */
    return languageMenu;
}



//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ScintillaEditor::setLanguage(QAction* langAction) {
/* -sets highlighting language based on 'langAction' */
    lexer->getLanguageData( langFileFromAction->value(langAction) );
    this->recolor();
}
