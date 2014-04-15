/*
Project: Lepton Editor
File: mainwindow.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: April 15, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of the main window object.

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
#include "mainwindow.h"
#include "ui_mainwindow.h"



//~public method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
/*
-class constructor builds the main window
*/
    ui->setupUi(this);  //reference the main window

    //hide the extra featuers
    ui->projectManager->hide();
    ui->editorTools->hide();

    //instantiate editing area
    editors = new EditorTabBar(this);
    ui->textEditorArea->insertWidget(0, editors);
    editors->addTab();

    //set language selector menu
    langSelector = NULL;        //null initial language selector to allow "dereferencing"
    setLanguageSelectorMenu();  //set language selector from editing tab instance

    connect(editors, SIGNAL(currentChanged(int)), this, SLOT(editTabChanged()) );
    connect(editors, SIGNAL(tabCloseRequested(int)), this, SLOT(editTabChanged()) );
    connect(editors, SIGNAL(saveSignal(int)), this, SLOT(save_signal_received(int)) );
    //connect(this, SIGNAL())
}

MainWindow::~MainWindow() {
    delete ui;
}



//~protected member implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::closeEvent(QCloseEvent *event) {
/* -called whenever a close window is requested */
    int err = editors->closeAll();  //request to close all open tabs
    if (err != 0) event->ignore();  //if a tab was not closed, do not close the window
    else event->accept();           //if all tabs where closed, close the main window
}



//~private slot implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::on_actionOpen_triggered() {
/*
-open a file in an editor tab
*/
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"));  //open pop-up window to prompt user for file to be opend
    if (!filePath.isEmpty()) {                              //if a file was selected
        if( !editors->current()->toPlainText().isEmpty() ) {    //check if the current tab contains text; if it does
            int i = editors->addTab();                              //create new tab
            editors->setCurrentIndex(i);                            //set the new tab as the current tab
        }
        editors->current()->loadFile(filePath);                 //load the file contents into the editor
    }
}

void MainWindow::on_actionNew_triggered() {
/* -open a new tab */
    editors->addTab();
}

void MainWindow::on_actionSave_triggered() {
/* -save content of tab to corresponding file */
    if ( editors->current()->getFileName().isEmpty() ) {    //if no file is being edited
        on_actionSave_As_triggered();                           //perform a 'save as' instead of a 'save'
        return;                                                 //
    }
    editors->current()->saveChanges();  //save changes of current editor
}

void MainWindow::save_signal_received(int index) {
/* -save contents of tab with specified index */
    if ( editors->getEditor(index)->getFileName().isEmpty() ) {    //if no file is being edited
        on_actionSave_As_triggered();                           //perform a 'save as' instead of a 'save'
        return;                                                 //
    }
    editors->getEditor(index)->saveChanges();  //save changes of current editor
}

void MainWindow::on_actionSave_As_triggered() {
/* -save content of tab to a new file */
    //editors->saveAs();
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As") );  //get a new file name
    if ( filePath.isEmpty() ) return;                                       //check if file name was specified
    QFile* file = new QFile(filePath);                                      //connect the new file
    file->open(QIODevice::WriteOnly | QIODevice::ReadWrite);                //open the new file in order to create it (in case it does not already exist)
    file->close();                                                          //
    editors->current()->saveChangesTo(file);                                //save changes to the new file
}

void MainWindow::on_actionSave_Copy_As_triggered(){
/* -save a copy of the content in the tab to a new file */
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Copy As") ); //get a new file name
    if ( filePath.isEmpty() ) return;                                           //check if file name was specified
    QFile* file = new QFile(filePath);                                          //open the new file
    file->open(QIODevice::WriteOnly | QIODevice::ReadWrite);                    //open the new file in order to create it (in case it does not already exist)
    file->close();                                                              //
    editors->current()->saveCopyOfChanges(file);                                //save changes to the new file
}

void MainWindow::on_actionProject_Manager_toggled(bool visible) {
/* -show/hide the project manager */
    ui->projectManager->setVisible(visible);
}

void MainWindow::on_actionEditor_Tools_toggled(bool visible) {
/* -show/hide the editor tools */
    ui->editorTools->setVisible(visible);
}

void MainWindow::on_actionSave_All_triggered() {
/* -save changes to all documents */
    for (int i = 0, l = editors->count(); i < l; i++) {
        if ( editors->getEditor(i)->getFileName().isEmpty() ) {
        //if no file is being edited
            //perform a 'save as' instead of a 'save'
            QString filePath = QFileDialog::getSaveFileName(this, tr("Save Copy As") ); //get a new file name
            if ( filePath.isEmpty() ) return;                                           //check if file name was specified
            QFile* file = new QFile(filePath);                                          //open the new file
            file->open(QIODevice::WriteOnly | QIODevice::ReadWrite);                    //open the new file in order to create it (in case it does not already exist)
            file->close();                                                              //
            editors->getEditor(i)->saveCopyOfChanges(file);                             //save changes to the new file
            return;                                                                     //
        }
        editors->getEditor(i)->saveChanges();   //save changes of current editor
    }
}

void MainWindow::editTabChanged() {
/* -called when visible tab is changed to update lanugage selector menu */
    setLanguageSelectorMenu();
}



//~private method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::setLanguageSelectorMenu() {
/* -set the language selector menu from editor object */
    if (editors->count() <= 0) {    //if no editor windows are open
        langSelector = NULL;            //do not point to any language selectors
        return;                         //do nothing else
    }

    if (langSelector != NULL)               //if a language selector is being pointed to (the language selector from previous tab)
        ui->menuBar->removeAction(langSelector);//stop pointing to it

    langSelector = ui->menuBar->addMenu( editors->current()->languageSelector->languageMenu );  //set and point to the new languages selector
}
