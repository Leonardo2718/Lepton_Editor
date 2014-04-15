/*
Project: Lepton Editor
File: editortabbar.cpp
Author: Leonardo Banderali
Created: February 9, 2014
Last Modified: April 15, 2014

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
#include <QDebug>



//~public method implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

EditorTabBar::EditorTabBar(QWidget *parent) : QTabWidget(parent) {
/*
-configure the widget and connect signals to slots
*/
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeEditor(int)) );

    setTabsClosable(true);
    setMovable(true);
    setDocumentMode(true);
}

EditorTabBar::~EditorTabBar() {
/* -dealocates (deletes) all the editor tabs stored in 'tabs' */
    /*for (int i = 0, s = tabs.size(); i < s; i++) {
        //closeEditor(i);    //close tab before deleting the object, %% this may be removed once I find a way to save sessions %%
        delete tabs.at(i);
    }*/
}

int EditorTabBar::addTab(int index) {
/*
-instantiate a new instance of the 'Editor' class, add it to 'tabs', and create
 an actual tab for the instance
-parameters:
    index: the index at which to place the new tab (-1 if at the end)
*/
    int i = 0;                                  //store the index of the new tab
    //int n = tabs.size() + 1;                    //get the new number tabs created
    //int n = this->count() + 1;
    if ( index < 0) {                           //if a new tab is to be appended to the end
        //tabs.append( new Editor(this) );            //create the new tab
        //i = QTabWidget::addTab(tabs.at(n-1), tabs.at(n-1)->getInnerFileName() );        //add the new tab
        //i = QTabWidget::addTab(tabs.at(n-1), "Untitled" );        //add the new tab
        i = QTabWidget::addTab( new Editor(this) , "Untitled" );
    }
    else {
        //tabs.insert(index, new Editor(this) );
        //i = QTabWidget::insertTab(index, tabs.at(n-1), tabs.at(n-1)->getInnerFileName() );
        //i = QTabWidget::addTab(tabs.at(n-1), "Untitled" );
        i = QTabWidget::insertTab(index, new Editor(this) , "Untitled" );
    }

    this->setCurrentIndex(i);
    connect(current(), SIGNAL(updateLabel(QString)), this, SLOT(setLabel(QString)) );
    return i;                   //return the index of the tab
}

Editor* EditorTabBar::current(){
/* -access current tab object */
    //return tabs[currentIndex()];
    return (Editor*)(currentWidget());
}

Editor* EditorTabBar::getEditor(int i) {
/* -access tab object using its index */
    //return tabs[i];
    return (Editor*)(widget(i));
}

/*int EditorTabBar::count() {
/* -returns the number of open editor tabs *
    return tabs.length();
}*/

int EditorTabBar::closeAll() {
/* -closes all open tabs/documents */
    int err = 0;
    for (int i = count() - 1; i >= 0; i--) {  //close all open tabs starting with the last
        err = closeEditor(i);
        if (err != 0) return err;
    }
    return 0;
}



//~pirvate slot implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void EditorTabBar::setLabel(const QString& label) {
/* sets current tab label */
    setTabText( currentIndex(), label);
}

int EditorTabBar::closeEditor(const int& index){
/*
-close tab of index 'index'
-returns:
    - '-2' if the close decision was canceled
    - '-1' if an unknown error has occured and the tab could not be closed
    - '0' if window was closed succesfully
*/
    if( ! getEditor(index)->wasFileSaved() ) {
    /* -if the file being closed was not saved, create a message box to ask the user what to do */

        //get the file name of the document being closed
        QString msg = "The file \"\" was not saved.  What would you like to do?";
        QString fileName = getEditor(index)->getFileName();
        if ( fileName.isEmpty() ) fileName = "Untitled";
        msg.insert(10, fileName);

        //create the message box
        int result = QMessageBox::warning(this, tr("Unsaved document!"), msg, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        //act according the action selected by the user
        switch (result) {
            case QMessageBox::Save:     //'Save' was clicked
                emit saveSignal(index);     //emit signal to try saving the document
                break;
            case QMessageBox::Discard:  //'Don't Save' was clicked
                break;                      //do not try saving
            case QMessageBox::Cancel:   //'Cancel' was clicked
                return -2;                  //do not do anything
                break;
            default:                    //should never be reached
                return -1;                  //do not do anything to avoid loss of data
                break;
        }
    }

    Editor* tmp = getEditor(index);  //a new pointer is created because the allocated memory must be deleted last
    disconnect(tmp, SIGNAL(updateLabel(QString)), this, SLOT(setLabel(QString)) );
    this->removeTab(index);   //I don't know why but for some reason, '1' is subtracted from the tab index when this method is called so a '1' needs to be added in order to prevent an out of range error
    //Editor* tmp = tabs[index];  //a new pointer is created because the allocated memory must be deleted last
    //tabs.remove(index);
    delete tmp;
    /*if (tabs.size() < 1) {
        addTab();
    }*/
    return 0;
}
