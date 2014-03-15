/*
Project: Lepton Editor
File: editortabbar.cpp
Author: Leonardo Banderali
Created: February 9, 2014
Last Modified: March 1, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of the 'EditorTabBar'.

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
#include "editortabbar.h"



//~public method implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

EditorTabBar::EditorTabBar(QWidget *parent) : QTabWidget(parent) {
/*
-configure the widget and connect signals to slots
*/
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(close(int)) );

    setTabsClosable(true);
    setMovable(true);
    setDocumentMode(true);
}

EditorTabBar::~EditorTabBar() {
/* -dealocates (deletes) all the editor tabs stored in 'tabs' */
    for (int i = 0, s = tabs.size(); i < s; i++) {

        /*###############################################################################
        ## Since 'SyntaxHighlighter' is instantiated using an 'Editor' object, objects ##
        ## of this type must be destructed first.                                      ##
        ###############################################################################*/

        delete highlighters.at(i);
        delete tabs.at(i);
    }
}

int EditorTabBar::addTab(int index) {
/*
-instantiate a new instance of the 'Editor' class, add it to 'tabs', and create
 an actual tab for the instance
-parameters:
    index: the index at which to place the new tab (-1 if at the end)
*/
    if ( index < 0) {                           //if a new tab is to be appended to the end
        tabs.append( new Editor(this) );            //create the new tab
        int s = tabs.size();                        //get the new number of open tabs
        highlighters.append( new SyntaxHighlighter( tabs.at(s-1)->document() ) );   //implement syntax highlighting
        QTabWidget::addTab(tabs.at(s-1), tabs.at(s-1)->getInnerFileName() );        //add the new tab
        return s;                                   //return the index of the tab
    }
    else {
        tabs.insert(index, new Editor(this) );
        int s = tabs.size();
        highlighters.insert(index, new SyntaxHighlighter( tabs.at(s-1)->document() ) );
        QTabWidget::insertTab(index, tabs.at(s-1), tabs.at(s-1)->getInnerFileName() );
        return s;
    }
}

int EditorTabBar::openFile(QString filePath) {
/* -open a file in an editor tab */
    int err = 0;

    if ( tabs.size() == 1 && current()->toPlainText() == "" ) { //if there is only one tab and it does not contain anything
        int i = currentIndex();                                     //get the index of the current tab
        err =  current()->loadFile( filePath );                     //load the file into the editor
        setTabText(i, current()->getInnerFileName() );              //set the lable of the tab
    }
    else {                                                      //otherwise
        addTab();                                                   //create a new tab/editor
        err =  tabs.last()->loadFile(filePath);                     //load the file into the editor
        setTabText(tabs.size()-1, tabs.last()->getInnerFileName() );//set the lable of the tab
    }

    return err;
}

int EditorTabBar::save() {
/* -save/write the contents of an editor tab to a file */
    if ( current()->getInnerFilePath().isEmpty() )
        saveAs();
    return current()->writeToFile();
}

int EditorTabBar::saveAs() {
/* -save/write the contents of an editor tab to a new file */
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As") );
    if(filePath.isEmpty()) return 0;
    int err = current()->writeToNewFile(filePath);
    setTabText( currentIndex(), current()->getInnerFileName() );
    return err;
}

int EditorTabBar::saveCopyAs() {
/* -save/write a copy of the contents of an editor tab to a new file */
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As") );
    return current()->writeToFile(fileName);
}

Editor* EditorTabBar::current(){
/* -access current tab object */
    return tabs[currentIndex()];
}

Editor* EditorTabBar::operator[](const int index) {
/* -directly access instances (tabs) of editor */
    return tabs[index];
}



//~piravte slot implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void EditorTabBar::close(int index){
/*
-close tab of index 'index'
*/
    QTabWidget::removeTab(index);
    delete highlighters.at(index);
    delete tabs.at(index);
    highlighters.remove(index);
    tabs.remove(index);
    if (tabs.size() < 1) {
        addTab();
    }
}

void EditorTabBar::languageSelected(QAction* a) {
/* -changes language to the one selected in the menu */
    //int tabIndex = currentIndex();      //get the index of the current tab
    QString langPath = "languages/";    //start the path to the language file
    if (a->text() == current()->languageSelector->actionList[0]->text() ){  //if the selected option is 'Plain Text'
        langPath = "";                                                          //set an empty path so that no language file is used
    }
    else{                                                                   //otherwise
        for (int i = 0, l = current()->languageSelector->actionList.length(); i < l; i++){  //for each action in the menu
            if(a->text() == current()->languageSelector->actionList[i]->text()) {             //if the action text is matched
                langPath.append( current()->languageSelector->langInfo[i-1].fileName );         //complete the path by adding the file name of the matched language selection action
                break;
            }
        }
    }

    QObject* parentEditor = a->parent()->parent();
    int tabIndex = indexOf((QWidget*)parentEditor);           //%%%%There is an error here %%%%%%

    highlighters[tabIndex]->useLanguage( langPath );    //use the selected file

    highlighters.at(tabIndex)->rehighlight();           //rehighlight the document
}
