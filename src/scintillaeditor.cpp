/*
Project: Lepton Editor
File: scintillaeditor.h
Author: Leonardo Banderali
Created: May 5, 2014
Last Modified: May 8, 2014

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

#include <QFile>
#include <QMessageBox>
#include <Qsci/qscilexercpp.h>

#include "scintillaeditor.h"



//~public method implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ScintillaEditor::ScintillaEditor(QWidget* parent) : QsciScintilla(parent) {
    //set and display line numbers; margin '1' is the default line number maring
    setMarginWidth(1, "00000");
    setMarginLineNumbers(1, true);
    //*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //$ Stub code used to test Scintilla features                          $$
    //$                                                                    $$
        setFolding(BoxedTreeFoldStyle);     //display line folding margins
        //QsciLexerCPP* cpp = new QsciLexerCPP();
        //setLexer(cpp);
        //QSettings s("Lepton", "Lepton");
        //char* c = new char('/Scintilla');
        //char* c = new char(' ');
        //cpp->writeSettings(s, c);
        //qDebug() << s.;
        lexer = new LeptonLexer(this);
        setLexer(lexer);
    //$                                                                    $$
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
}

ScintillaEditor::~ScintillaEditor() {
    setLexer(); //clear lexer use before deleting instance
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
    file.write( this->text().toUtf8() );                     //write edited text
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
