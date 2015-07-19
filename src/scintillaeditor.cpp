/*
Project: Lepton Editor
File: scintillaeditor.h
Author: Leonardo Banderali
Created: May 5, 2014
Last Modified: July 19, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of a subclass of QsciScintilla.  This class will be used as
    editing environment instead of the default QPlainTextEdit class.

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

//include Qt classes
#include <QApplication>
#include <QFile>
#include <QFont>
#include <QDir>
#include <QList>
#include <QDomDocument>
#include <QMessageBox>
#include <Qsci/qscilexercpp.h>

//include other Lepton classes and objects
#include "scintillaeditor.h"
#include "leptonconfig.h"



//~public method implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ScintillaEditor::ScintillaEditor(QWidget* parent) : QsciScintilla(parent) {
/* -setup editor and configurations */
    //set and display line numbers; margin '1' is the default line number maring
    setMarginWidth(1, 55);
    setMarginLineNumbers(1, true);

    //create the lexer manager
    lexerManager = new SyntaxHighlightManager(this);

    //set editor properties/settings
    setAutoIndent(true);
    setTabWidth(4);
    setMarginsBackgroundColor( LeptonConfig::mainSettings->getValueAsColor("editor_theme", "margins_background") );
    setMarginsForegroundColor( LeptonConfig::mainSettings->getValueAsColor("editor_theme", "margins_foreground") );
    setWhitespaceVisibility( LeptonConfig::mainSettings->getWhiteSpaceVisibility() );
    setWhitespaceForegroundColor( LeptonConfig::mainSettings->getValueAsColor("editor_theme", "whitespace_color") );
    setCaretForegroundColor( LeptonConfig::mainSettings->getValueAsColor("editor_theme", "caret_color") );
    setCallTipsHighlightColor( LeptonConfig::mainSettings->getValueAsColor("editor_theme", "highlight_color") );
    setSelectionBackgroundColor( LeptonConfig::mainSettings->getValueAsColor("editor_theme", "selection_background") );
    setSelectionForegroundColor( LeptonConfig::mainSettings->getValueAsColor("editor_theme", "selection_foreground") );
    setIndentationsUseTabs(false);  //use spaces instead of tabs for indentation

    //*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //$ Stub code used to test Scintilla features                          $$
    //$                                                                    $$

    //$                                                                    $$
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
}

ScintillaEditor::~ScintillaEditor() {
/* clean up and delete allocated memory */
    //delete allocated memory
    delete lexerManager;
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

    //do not consider the file as having been modified (it was just opened)
    setModified(false);

    //set a lexer for the new file
    lexerManager->setLexerForFile( file.fileName() );
}

bool ScintillaEditor::isFileOpen() {
/*  -returns true if a file is open and being edited, false otherwise */
    return openFile.exists();   //the only time a file is open is when it's defined (path not empty) and it exists
}

QString ScintillaEditor::getOpenFilePath() {
/* -get the path to the file currently being edited */
    return openFile.absoluteFilePath();
}

QString ScintillaEditor::getOpenFileDir() {
/*  -get the directory of the file currently being edited */
    return openFile.absolutePath();
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
    return lexerManager->getLanguageMenu();
}



//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ScintillaEditor::changeTabsToSpaces() {
/*  -changes indentation tabs into spaces */
    //go through the text line by line and replace tabs with spaces
    QStringList editorTextLines = text().split( QRegularExpression("\n") );

    QString replaceText("");
    for (int i = 0, l = tabWidth(); i < l; i++) {
        replaceText.append(' ');
    }

    for (int i = 0, l = editorTextLines.length(); i < l; i++) {
        editorTextLines[i].replace('\t', replaceText);
    }

    setText( editorTextLines.join("\n") );
}

void ScintillaEditor::changeSpacesToTabs() {
/*  -changes spaces into tabs */
    //go through the text line by line and replace spaces with tabs
    QStringList editorTextLines = text().split( QRegularExpression("\n") );

    QString textExpression = tr("\\s{2,%1}").arg( tabWidth() );

    for (int i = 0, l = editorTextLines.length(); i < l; i++) {
        editorTextLines[i].replace( QRegularExpression(textExpression), "\t");
    }

    setText( editorTextLines.join("\n") );
}

/*
remove the spaces at the end of each line
*/
void ScintillaEditor::removeTrailingSpaces() {
    setText( text().replace(QRegularExpression("[ \t]+\n"), "\n") );    // remove trailing spaces (except last line)
    setText( text().remove(QRegularExpression("[ \t]+$")) );            // remove trailing spaces in last line

    /*######################################################################################
    ### Note that these are not combined into a single step because doing so would cause  ##
    ### an extra '\n' do be placed at the end of the file if not already present.  This   ##
    ### functionallity should be implemented separately.                                  ##
    ######################################################################################*/
}
