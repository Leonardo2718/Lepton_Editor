/*
Project: Lepton Editor
File: editortabbar.cpp
Author: Leonardo Banderali
Created: February 9, 2014
Last Modified: May 17, 2014

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
    if ( index < 0) {
        i = QTabWidget::addTab( new ScintillaEditor() , "Untitled" );
    }
    else {
        i = QTabWidget::insertTab(index, new ScintillaEditor() , "Untitled" );
    }

    this->setCurrentIndex(i);
    connect(current(), SIGNAL(modificationChanged(bool)), this, SLOT(setLabel(bool)) );
    return i;                   //return the index of the tab
}

ScintillaEditor* EditorTabBar::current(){
/* -access current tab object */
    return (ScintillaEditor*)(currentWidget());
}

ScintillaEditor* EditorTabBar::getEditor(int i) {
/* -access tab object using its index */
    return (ScintillaEditor*)(widget(i));
}

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

void EditorTabBar::setLabel(bool modifiedStatus) {
/* sets current tab label */
    //QString label = current()->getOpenFileName();
    ScintillaEditor* e = (ScintillaEditor*)(sender());
    QString label = e->getOpenFileName();
    if ( label.isEmpty() ) label = "Untitled";
    if ( modifiedStatus == true ) label = label.prepend('*');   //prepend '*' to show that file has not been saved
    setTabText( indexOf(e), label);
}

int EditorTabBar::closeEditor(const int& index){
/*
-close tab of index 'index'
-returns:
    - '-2' if the close decision was canceled
    - '-1' if an unknown error has occured and the tab could not be closed
    - '0' if tab was closed succesfully
*/
    //if the file being closed was not saved, create a message box to ask the user what to do
    if( ! getEditor(index)->wasFileSaved() ) {

        //get the file name of the document being closed
        /*QString msg = "The file \"\" was not saved.  What would you like to do?";
        QString fileName = getEditor(index)->getFileName();
        if ( fileName.isEmpty() ) fileName = "Untitled";
        msg.insert(10, fileName);*/
        QString msg = tr("The following file was not saved: %1\nWhat would you like to do?").arg( tabText(index) );

        //create the message box
        int result = QMessageBox::warning(this, tr("Unsaved document!"), msg, QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);

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

    //Editor* tmp = getEditor(index);  //a new pointer is created because the allocated memory must be deleted last
    ScintillaEditor* tmp = getEditor(index);//a new pointer is created because the allocated memory must be deleted last
    disconnect(tmp, SIGNAL(modificationChanged(bool)), this, SLOT(setLabel(bool)) );
    this->removeTab(index);   //I don't know why but for some reason, '1' is subtracted from the tab index when this method is
    delete tmp;
    /*if (tabs.size() < 1) {
        addTab();
    }*/
    return 0;
}
