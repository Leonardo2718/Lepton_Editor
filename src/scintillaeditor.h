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

    This file contains the definition of a subclass of QsciScintilla.  This class will be used as
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

#ifndef SCINTILLAEDITOR_H
#define SCINTILLAEDITOR_H

#include <QWidget>
#include <QString>
#include <QFileInfo>
#include <QSettings>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercustom.h>
#include "leptonlexer.h"

class ScintillaEditor : public QsciScintilla
{
    Q_OBJECT

    public:
        explicit ScintillaEditor(QWidget* parent = 0);

        ~ScintillaEditor();

        void writeToFile(const QString& filePath, bool changeModify = false);
        /*
        -writes edited text to a file
        -parameters:
            filePath: bath to file
            changeModify: if true, call setModified() to change the modified state, otherwise do nothing
        */

        void loadFile(const QString& filePath);
        /* -load contents of a file to be edited */

        QString getOpenFilePath();
        /* -get the path to the file currently being edited */

        QString getOpenFileName();
        /* -get the name of the file currently being edited */

        bool wasFileSaved();
        /* returns wheater changes to the open file have been saved/writen */

    private:
        QFileInfo openFile; //path to file currently being edited

        void setModified(bool m) { QsciScintilla::setModified(m); }    //make method private

        LeptonLexer* lexer; //lexer to be used for highlighting

};

#endif // SCINTILLAEDITOR_H
