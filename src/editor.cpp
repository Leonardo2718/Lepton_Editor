/*
Project: Lepton Editor
File: editor.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: March 15, 2014

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

Editor::Editor(QTabWidget *parent, QString filePath) : QPlainTextEdit(parent){//, parentTab(parent) {
/*
-this constructor connects signals to coresponding slots
*/
    numArea = new LineNumberArea(this);
    highlighter = new SyntaxHighlighter(this->document());
    languageSelector = new LanguageSelectorClass(this);
    contentFile = new QFile(filePath);
    saveStatus = false; //when an instance of the editor is created, no file should have been saved

    //connect slots to signals in 'QPlainTextEdit'
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateAreaWidth()));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightEditorLine()));
    connect(languageSelector->actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeLanguage(QAction*)) );
    connect(this, SIGNAL(textChanged()), this, SLOT(markNotSaved()) );

    //setup editing area
    updateAreaWidth();      //set line number area
    highlightEditorLine();  //set editor line highlighting
    setLineWrapMode(QPlainTextEdit::NoWrap);    //do not wrap text
    setFont( QFont("Monospace", 9) );           //use an appropriate font
    setTabStopWidth( fontMetrics().width(QLatin1Char(' '))*4);  //set tab width to 4 spaces

    /*if (filePath == 0) {        //if no file is specified
        innerFileName = "Untitled"; //use the defualt name
        innerFilePath.clear();      //clears the file path
    }
    else
        loadFile(filePath);
    */
    if ( !filePath.isEmpty() ) loadFile(filePath);  //if a file path is specified, open the file
}

Editor::~Editor() {
/* -deletes allocated memory */
    delete numArea;
    delete highlighter;
    delete contentFile;
    delete languageSelector;
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
        max /= 10;          //devide the number by '10'
        digits++ ;          //add one to the digit count
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
            painter.drawText(0, top, numArea->width(), fontMetrics().height(), Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int Editor::loadFile(QString filePath) {
/*
-open a file and set the contents of the file as the inner text
-returns:
    - '-3' if no file was specified
    - '-2' if the file specified does not exist
    - '-1' if the file could not be opened for an unknown reason
    - '0' if the file was successfully loaded
*/
    if ( filePath.isEmpty() ) return -3;    //if no file is specified, return an error

    contentFile->setFileName(filePath);     //select the file
    if ( !contentFile->exists() ) return -2;//if the file does not exist, return an error

    if ( contentFile->open(QIODevice::ReadWrite | QIODevice::ReadOnly) ) {  //if the file can be successfully opened
        QTextStream text(contentFile);                                          //create a text stream to read the file as a string
        setPlainText( text.readAll() );                                         //read the file
        contentFile->close();                                                   //close the file
        markSaved();                                                            //since the files was just opened there is no need to save anything
        return 0;                                                               //return with no error
    }

    return -1;   //return unknown error if nothing could be done with the file
}

int Editor::writeToFile(QFile* file) {
/*
-write edited text to a file
-returns:
    - '-2' if the specified file does not exist
    - '-1' if a writing error occured
    -the number of bytes writen if the write was successful
*/
    if ( !file->exists() ) return -2;                                   //if the file does not exist, return an error
    if ( file->open(QIODevice::ReadWrite | QIODevice::WriteOnly) ) {    //if the file can be successfully opened
        file->resize( toPlainText().size() );                               //resize the file to the size of the new data
        int err = file->write( toPlainText().toUtf8() );                    //write inner text to the file
        file->close();
        return err;                                                         //return error status
    }
    return -1;
}

int Editor::saveChanges() {
/* -save changes made to the file */
    int err = writeToFile(contentFile); //write changes
    if (err >= 0) markSaved();                          //if the save was successful, mark the changes as saved
    return err;                         //return error status
}

int Editor::saveChangesTo(QFile* file) {
/* -save changes to a different file and load the file (Save As) */
    if ( !file->exists() ) return -2;                    //check if the specified file exists
    if ( contentFile->isOpen() ) contentFile->close();  //make sure that the previous file is closed
    contentFile = file;                                 //select the new file
    int err = writeToFile(contentFile);                 //write to the file
    if (err >= 0) markSaved();                          //if the save was successful, mark the changes as saved
    return err;                                         //return error status
}

int Editor::saveCopyOfChanges(QFile* file) {
/*
-save a copy of the changes to a different file (Save Copy As)
-Note that changes are not marked as saved because only a copy of the file with the changes was saved
 but not the file itself
*/
    if ( !file->exists() ) return -2;   //check if the specified file exists
    int err = writeToFile(file);        //write to the file
    return err;                         //return error status
}

QString Editor::getFileName() {
/* -returns the name and path to the file being edited */
    return contentFile->fileName();
}

bool Editor::wasFileSaved() {
/* -returns whether the changes have been saved */
    return saveStatus;
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


void Editor::highlightEditorLine() {
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

void Editor::changeLanguage(QAction* a) {
/* -change syntax highlighting language */
    QString langPath = languageSelector->getLangFileFor(a); //get path to new language file
    highlighter->useLanguage( langPath );                   //use the new file
    highlighter->rehighlight();                             //rehighlight the document
}

void Editor::markSaved() {
/* -sets variable 'saveStatus' to true and emits signal of the change */
    if (saveStatus == true) return;     //no need to do anything if the save status has not changed
    saveStatus = true;                  //change state
    emit saveStatusChanged(saveStatus); //emit signal to notify other objects of the chage
}

void Editor::markNotSaved() {
/* -sets variable 'saveStatus' to false and emits signal of the change */
    if (saveStatus == false) return;    //no need to do anything if the save status has not changed
    saveStatus = false;                 //change state
    emit saveStatusChanged(saveStatus); //emit signal to notify other objects of the chage
}


//~'LanguageSelectorClass' class implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//declare static members
int LanguageSelectorClass::instanceCount = 0;
QVector< LanguageSelectorClass::FileInfoType > LanguageSelectorClass::langInfo;

LanguageSelectorClass::LanguageSelectorClass(QWidget *_parent) {
/*
-Constuctor keeps track of the number of class instances.  If the first
 instance of this class is being instanciated, then this method checks
 all the files in the languages directory and retrieves data for the
 language selector menu.
*/
    instanceCount++;    //update class instance count

    languageMenu = new QMenu("Languages",_parent);
    actionGroup = new QActionGroup(languageMenu);

    if(instanceCount == 1) {            //if this is the first class instance
        QDir langDir("languages/");         //access the languages director to scan the files
        QStringList nameFilterList;         //create filter to only scan xml files
        nameFilterList.append("*.xml");     //

        /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        //$ For whatever reason, this line of code causes a 'malloc(): memory corruption (fast)' $$$
        //$ error so the above line was used instead.                                            $$$
        //$                                                                                      $$$
            nameFilterList << "*.xml";
        //$                                                                                      $$$
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

        QStringList fileList = langDir.entryList(nameFilterList);//get a list of all the file names which match the filter in order to scan them
        int fileListLength = fileList.length();

        langInfo.clear();                       //clear the current list
        langInfo.resize(fileListLength);        //resize the info list to fit all the data read from files

        for (int i = 0; i < fileListLength; i++){   //for each file in the 'languages' directory
            //open the file
            QFile langFile( langDir.filePath(fileList[i]) );
            if ( !langFile.open(QIODevice::ReadOnly | QIODevice::Text) ) continue;

            //create a DOM object for the file
            QDomDocument languageDocument("lang_file");         //create a DOM object from the data file
            bool check = languageDocument.setContent(&langFile);//set content of the DOM object from the language data file
            langFile.close();                                   //close the file
            if (!check) continue;                               //if the file could not be parsed, return
            QDomElement languageElement = languageDocument.documentElement();   //extract root element
            if( languageElement.nodeName() != "language" ) continue;            //verify that the root element is correct

            //extract info and create menu action
            QString langName = languageElement.attribute("name");           //extract attribute with the name of the language
            if( langName.isEmpty() || langName.isNull()) continue;          //verify that the 'Name' attribute exists
            langInfo[i].filePath = langDir.absoluteFilePath(fileList[i]);   //store file path name
            langInfo[i].languageName = langName;                            //store the extracted language name
        }
    }

    actionList.resize(langInfo.length() + 1);  //resize with an additional element for 'Plain Text' selection

    //add first action for 'Plain Text' selection
    actionList[0] = new QAction("Plain Text", languageMenu);
    actionList[0]->setCheckable( 1 );
    actionList[0]->setChecked( 1 );
    actionGroup->addAction(actionList[0]);
    languageMenu->addAction(actionList[0]);

    //create other actions for each language file
    for (int i = 1, l = langInfo.length(); i <= l; i++) {
        actionList[i] = new QAction(langInfo[i-1].languageName, languageMenu);  //create and add menu action from on extracted info
        actionList[i]->setCheckable( 1 );                       //
        actionGroup->addAction(actionList[i]);                  //
        languageMenu->addAction(actionList[i]);                 //
    }
}

LanguageSelectorClass::~LanguageSelectorClass() {
/* -free allocated memory */
    for (int i = 0, l = actionList.length(); i < l; i++){
        delete actionList[i];
    }
    instanceCount--;
}

QString LanguageSelectorClass::getLangFileFor(QAction* a) {
/* -get the path to the language file which corresponds to the menu action 'a' */
    QString path;    //path to file
    if (a->text() == actionList[0]->text() ){  //if the selected option is 'Plain Text'
        path = "";                                  //set an empty path so that no language file is used
    }
    else{                                       //otherwise
        int l = actionList.length();
        for (int i = 0; i < l; i++){                //for each action in the menu
            if(a->text() == actionList[i]->text()) {    //if the action text is matched
                path = langInfo[i-1].filePath;          //get the path for the corresponding language file
                break;
            }
        }
    }

    return path;
}
