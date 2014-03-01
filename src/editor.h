/*
Project: Lepton Editor
File: editor.h
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: March 1, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the definition of the 'Editor' class which inherites from 'QPlainTextEdit'.
    This is the object used to edit text.

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

#ifndef EDITOR_H    //prevent file from being included more than once
#define EDITOR_H

//include necessary files and libraries
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QtWidgets>

class LineNumberArea;   //allow CEditor to know about 'LineNumberArea'

class Editor : public QPlainTextEdit {
    Q_OBJECT    //implement QObject

    public:
        explicit Editor(QTabWidget* parent, QString filePath = 0);
        /* -constructor connects signals to coresponding slots and opens a file (if specified) */

        ~Editor();
        /* -deletes allocated memory */

        int areaWidth();
        /* -calculates the width of the line number aread based on the number of digits in the number of the last line
            of the file and the maximum width of a digit
           -this method needs to be public because it is used by the 'LineNumberArea' class
        */

        void lineNumberAreaPaintEvent(QPaintEvent *event);

        const QString& getInnerFileName() const;
        /* -return the name of the inner text file */

        const QString& getInnerFilePath() const;
        /* -return the path to the inner text file */

        int loadFile(QString filePath);
        /*
        -open a file and set the contents of the file as the inner tex
        -returns 0 if file was opened successfully or 1 if the file could not be opened
        */

        int writeToFile(QString filePath = 0);
        /* -save editor text to a file */

        int writeToNewFile(QString filePath = 0);
        /* -save editor text to a new file */

    public slots:
        void markUnsaved();
        /* -adds an asterisk (*) to the start of the file name if inner text is changed and not yet saved */

    protected:
        void resizeEvent(QResizeEvent *event);
        /* -reseize the line number area (vertically) when the code editor is resized */

    private slots:
        void updateAreaWidth();
        /* -set the margin on the editor where the line number will be printed (painted) */

        void updateLineNumberArea(const QRect& rect, int dy);
        /* -slot is invoked when the editors viewport has been scrolled */

        void highlightCurrentLine();
        /* -highlight the line which contains the cursor */

    private:
        QTabWidget* parentTab;//constant pointer to the parrent widget

        LineNumberArea *numArea;    //pointer to the area which will be painted with the line numbers
        //friend class LineNumberArea;

        QString innerFileName;  //holds the name of the file opened in the editor instance ("Untitled" if no file is opened)
        QString innerFilePath;  //holds the path to the file opened (null if no file is opened)


};



//~'LineNumberArea' class definition and implementation~~~~~~~~~~~~~~~~~~~~~~~~

class LineNumberArea : public QWidget {
    public:
        LineNumberArea(Editor *editor) : QWidget(editor) {
            codeEditor = editor;
        }

        QSize sizeHint() const {
            return QSize(codeEditor->areaWidth(), 0);
        }

    protected:
        void paintEvent(QPaintEvent *event) {
            codeEditor->lineNumberAreaPaintEvent(event);
        }

    private:
        Editor *codeEditor;
};

#endif // EDITOR_H
