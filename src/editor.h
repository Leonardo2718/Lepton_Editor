/*
Project: Lepton Editor
File: editor.h
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: March 15, 2014

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
#include <QActionGroup>
#include <QMenu>
#include <QDir>
#include <QFile>
#include <syntaxhighlighter.h>

class LineNumberArea;   //allow Editor to know about 'LineNumberArea'
class LanguageSelectorClass;

class Editor : public QPlainTextEdit {
    Q_OBJECT    //implement QObject

    public:
        explicit Editor(QTabWidget* parent, QString filePath = 0);
        /* -constructor connects signals to coresponding slots and opens a file (if specified) */

        LanguageSelectorClass* languageSelector;

        ~Editor();
        /* -deletes allocated memory */

        int areaWidth();
        /* -calculates the width of the line number aread based on the number of digits in the number of the last line
            of the file and the maximum width of a digit
           -this method needs to be public because it is used by the 'LineNumberArea' class
        */

        void lineNumberAreaPaintEvent(QPaintEvent *event);

        //const QString& getInnerFileName() const;
        /* -return the name of the inner text file */

        //const QString& getInnerFilePath() const;
        /* -return the path to the inner text file */

        int loadFile(QString filePath);
        /*
        -open a file and set the contents of the file as the inner text
        -returns:
            - '-3' if no file was specified
            - '-2' if the file specified does not exist
            - '-1' if the file could not be opened for an unknown reason
            - '0' if the file was successfully loaded
        */

        int writeToFile(QFile* file);
        /*
        -write edited text to a file
        -returns:
            - '-2' if the specified file does not exist
            - '-1' if a writing error occured
            -the number of bytes writen if the write was successful
        */

        int saveChanges();
        /* -save changes done to the file */

        int saveChangesTo(QFile* file);
        /* -save changes to a different file and load the file (Save As) */

        int saveCopyOfChanges(QFile* file);
        /* -save a copy of the changes to a different file (Save Copy As) */

        //int writeToNewFile(QString filePath = 0);
        /* -save editor text to a new file */

        QString getFileName();
        /* -returns the name and path to the file being edited */

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

        void highlightEditorLine();
        /* -highlight the line which contains the cursor */

        void changeLanguage(QAction* a);
        /* -change syntax highlighting language */

    private:
        QTabWidget* parentTab;  //constant pointer to the parrent widget
        SyntaxHighlighter* highlighter; //highlighter for current document

        LineNumberArea* numArea;//pointer to the area which will be painted with the line numbers

        //QString innerFileName;  //holds the name of the file opened in the editor instance ("Untitled" if no file is opened)
        //QString innerFilePath;  //holds the path to the file opened (null if no file is opened)

        QFile* contentFile;     //holds the file which is being edited
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



//~'LanguageSelectorClass' class definition~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LanguageSelectorClass {
/*
This class is responsible for helding all the data related to language files
and language selections.  Some of the data includes a list of all the language
files.
*/
    public:
        explicit LanguageSelectorClass(QWidget *_parent);

        ~LanguageSelectorClass();

        struct FileInfoType {
        /* -struct to hold information extracted from a language file*/
            QString filePath;
            QString languageName;
        };

        static QVector< FileInfoType > langInfo;    //static list of all the language file names and language names
                                                    //  can be accessed by all instances of this class
        QMenu* languageMenu;                        //menu object to implement language selector
        QVector< QAction* > actionList;             //list of pointers to language actions
        QActionGroup* actionGroup;                  //object to store the group of actions and implement the language selector

        QString getLangFileFor(QAction* a);
        /* -get the path to the language file which corresponds to the menu action 'a' */

        int getInstanceCount(){
            return instanceCount;
        }

    private:
        static int instanceCount;
};

#endif // EDITOR_H
