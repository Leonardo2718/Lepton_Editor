/*
Project: Lepton Editor
File: editortabbar.cpp
Author: Leonardo Banderali
Created: February 9, 2014
Last Modified: April 2, 2014

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

        //delete highlighters.at(i);
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
    int i = 0;                                  //store the index of the new tab
    int n = tabs.size() + 1;                    //get the new number tabs created
    if ( index < 0) {                           //if a new tab is to be appended to the end
        tabs.append( new Editor(this) );            //create the new tab
        //i = QTabWidget::addTab(tabs.at(n-1), tabs.at(n-1)->getInnerFileName() );        //add the new tab
        i = QTabWidget::addTab(tabs.at(n-1), "Untitled" );        //add the new tab
    }
    else {
        tabs.insert(index, new Editor(this) );
        //i = QTabWidget::insertTab(index, tabs.at(n-1), tabs.at(n-1)->getInnerFileName() );
        i = QTabWidget::addTab(tabs.at(n-1), "Untitled" );
    }

    this->setCurrentIndex(i);
    connect(current(), SIGNAL(updateLabel(QString)), this, SLOT(setLabel(QString)) );
    return i;                   //return the index of the tab
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

void EditorTabBar::setLabel(const QString& label) {
/* sets current tab label */
    setTabText( currentIndex(), label);
}

void EditorTabBar::close(int index){
/*
-close tab of index 'index'
*/
    QTabWidget::removeTab(index);
    disconnect(tabs.at(index), SIGNAL(updateLabel(QString)), this, SLOT(setLabel(QString)) );
    delete tabs.at(index);
    tabs.remove(index);
    if (tabs.size() < 1) {
        addTab();
    }
}
