/*
Project: Lepton Editor
File: editor.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: March 1, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of the 'Editor' class which is used
    as the editing window.

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

//include necessary files and libraries
#include "editor.h"



//~public method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Editor::Editor(QTabWidget *parent, QString filePath) : QPlainTextEdit(parent), parentTab(parent) {
/*
-this constructor connects signals to coresponding slots
*/
    numArea = new LineNumberArea(this);

    //connect slots to signals in 'QPlainTextEdit'
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateAreaWidth()));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    //connect(this, SIGNAL(textChanged()), this, SLOT(markUnsaved()) );

    updateAreaWidth();
    highlightCurrentLine();

    setLineWrapMode(QPlainTextEdit::NoWrap);    //do not wrap text
    setFont( QFont("Monospace", 9) );           //use an appropriate font

    if (filePath == 0) {        //if no file is specified
        innerFileName = "Untitled"; //use the defualt name
        innerFilePath.clear();      //clears the file path
    }
    else
        loadFile(filePath);
}

Editor::~Editor() {
/* -deletes allocated memory */
    delete numArea;
}

int Editor::areaWidth() {
/*
-calculates the width of the line number aread based on the number of digits in the number of the last line
 of the file and the maximum width of a digit
-this method needs to be public because it is used by the 'LineNumberArea' class
*/
    uint8_t digits = 1;                         //variable to store the number of digits
    unsigned int max = qMax(1, blockCount() );  //retrieve and store the number of the last line in the file
                                                //  (the last number is assumed to be the largest of all numbers
                                                //  and therefore the one with the most digits)

    //count the number of digits
    while (max >= 10) { //until the number is less than '10'
        max /= 10;          //=>devide the number by '10'
        digits++ ;          //=>add one to the digit count
    }

    int space = fontMetrics().width(QLatin1Char('9')) * digits; //calculate space needed to print all the digits
                                                                //  of the last line number

    return space + 3;   //return the space needed to print the number with an extra '3' spaces for styling
}


void Editor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(numArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, numArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}


const QString& Editor::getInnerFileName() const {
/* -return the name of the inner text file */
    return innerFileName;
}


const QString& Editor::getInnerFilePath() const {
/* -return the path to the inner text file */
    return innerFilePath;
}


int Editor::loadFile(QString filePath) {
/*
-open a file and set the contents of the file as the inner tex
-returns 0 if file was opened successfully or 1 if the file could not be opened
*/
    QFile file(filePath);
    if ( !file.open(QIODevice::ReadWrite|QIODevice::ReadOnly)) {    //if the file could not be opened
        innerFilePath.clear();                                          //set the path to blank
        innerFileName = "Untitled";                                     //set the file name to default
        return 1;                                                       //return with an error
    }

    innerFilePath = filePath;                                   //store the path to the file
    innerFileName = filePath.split( QRegExp("/|\\\\") ).last(); //retrieve and store the file name

    QTextStream in(&file);          //create a text stream to read the file as a string
    setPlainText( in.readAll() );   //read the file
    file.close();                   //close the file

    return 0;                       //return with no error
}

int Editor::writeToFile(QString filePath) {
/* -save editor text to a file */
    if ( innerFileName.at(0) == '*' )   //if the first character is an asterisk
        innerFileName.remove(0, 1);         //remove it

    if ( filePath.isEmpty() ) {     //if no file name is specified, use internal file name
        filePath = innerFilePath;
    }

    QFile file(filePath) ;                  //open the file
    if ( !file.open(QIODevice::ReadWrite|QIODevice::WriteOnly) ) return 1;     //if the file could not be opened, return an error

    file.resize( toPlainText().size() );    //resize the file to the size of the new data
    file.write( toPlainText().toUtf8() );   //write inner text to the file
    file.close();

    return file.WriteError;                 //return any errors
}

int Editor::writeToNewFile(QString filePath) {
/* -save editor text to a new file */
    innerFilePath = filePath;
    innerFileName = filePath.split( QRegExp("/|\\\\") ).last();
    return writeToFile();
}



//~protected member definitions and implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Editor::resizeEvent(QResizeEvent *e) {
/*
-reseize the line number area (vertically) when the code editor is resized
*/
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    numArea->setGeometry(QRect(cr.left(), cr.top(), areaWidth(), cr.height()));
}



//~private slot definitions and implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Editor::updateAreaWidth() {
/*
-set the margin on the editor where the line number will be printed (painted)
*/
    /*#######################################################################################
    ## The area where the line numbers will be printed (painted) is created by setting the ##
    ## left margin of the code editor to the size calculated using 'setViewportMargins',   ##
    ## which is inherited from the 'QAbstractScrollArea' class.                            ##
    #######################################################################################*/

    setViewportMargins(areaWidth(), 0, 0, 0);   //set the left margin to the size required to fit line numbers
}


void Editor::updateLineNumberArea(const QRect& rect, int dy) {
/*
-slot is invoked when the editors viewport has been scrolled
-parameters:
    rect: the part of the code editor to be updated (redrawn) (the 'QRect' class defines a rectangle
          in a plane; the area where the line numbers are)
    dy: number of pixels the code editor has been scrolled vertically
*/
    if (dy)
        numArea->scroll(0, dy);
    else
        numArea->update(0, rect.y(), numArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateAreaWidth();
}


void Editor::highlightCurrentLine() {
/*
-highlight the line which contains the cursor
*/
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;                //variable to store a selection
        QColor lineColor = QColor(Qt::blue).lighter(195);   //define the color of the selected line

        selection.format.setBackground(lineColor);  //set the line color
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);    //select full row (line) at selection (cursor)
        selection.cursor = textCursor();            //set the selection to be at the cursor position
        selection.cursor.clearSelection();          //clear the selection before coloring line (to avoid highlighting
                                                    //  multiple lines)
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void Editor::markUnsaved() {
/* -adds an asterisk (*) to the start of the file name if inner text is changed and not yet saved */
    if (innerFileName.at(0) != '*') //if there is no asterisk yet
        innerFileName.prepend('*');     //add the asterisk

    int i = parentTab->indexOf(this);   //retrieve index of the tab coresponding to this object
    parentTab->setTabText(i, innerFileName);
}
