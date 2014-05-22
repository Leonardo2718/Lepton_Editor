/*
Project: Lepton Editor
File: mainwindow.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: May 21, 2014

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
#include "generalconfig.h"



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

    //create a new editor
    int tabIndex = editors->addTab();
    //ui->menuBar->insertMenu( ui->menuCustomize->menuAction(), editors->getEditor(tabIndex)->getLanguageMenu() );

    //set language selector menu
    //langSelector = NULL;        //null initial language selector to allow "dereferencing"
    setLanguageSelectorMenu();  //set language selector from editing tab instance

    connect(editors, SIGNAL(currentChanged(int)), this, SLOT(editTabChanged()) );
    connect(editors, SIGNAL(tabCloseRequested(int)), this, SLOT(editTabChanged()) );
    connect(editors, SIGNAL(saveSignal(int)), this, SLOT(save_signal_received(int)) );

    QString styleSheet;
    GeneralConfig::getStyleSheetInto(styleSheet);
    qApp->setStyleSheet(styleSheet);

    //ui->statusBar->
}

MainWindow::~MainWindow() {
    /*for ( int i = 0, c = editors->count(); i < c; i++) {
        ui->menuBar->removeAction( editors->getEditor(i)->getLanguageMenu()->menuAction() );
    }*/
    delete ui;
}



//~protected member implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::closeEvent(QCloseEvent *event) {
/* -called whenever a close window is requested */
    disconnect(editors, SIGNAL(currentChanged(int)), this, SLOT(editTabChanged()) );
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
    /*if (!filePath.isEmpty()) {                              //if a file was selected
        if( !editors->current()->toPlainText().isEmpty() ) {    //check if the current tab contains text; if it does
            int i = editors->addTab();                              //create new tab
            editors->setCurrentIndex(i);                            //set the new tab as the current tab
        }
        editors->current()->loadFile(filePath);                 //load the file contents into the editor
    }*/
    if ( filePath.isEmpty() ) return;
    if ( (editors->count() < 1) || (! editors->current()->text().isEmpty()) ) { //if text is already presend in the current editor, create a new tab
        qint8 i = editors->addTab();
        editors->setCurrentIndex(i);
    }
    editors->current()->loadFile(filePath);         //insert text into editor
}

void MainWindow::on_actionNew_triggered() {
/* -open a new tab */
    int tabIndex = editors->addTab();
    ui->menuBar->insertMenu( ui->menuCustomize->menuAction(), editors->getEditor(tabIndex)->getLanguageMenu() );
    setLanguageSelectorMenu();
}

void MainWindow::on_actionSave_triggered() {
/* -save content of tab to corresponding file */
    /*QString file = editors->current()->getOpenFilePath();
    if ( file.isEmpty() ) on_actionSave_As_triggered(); //if no file is currently open, perform a 'save as' instead

    //editors->current()->saveChanges();  //save changes of current editor
    editors->current()->writeToFile(file, true);        //save changes and mark file as saved*/
    saveFile( editors->currentIndex() );
}

void MainWindow::save_signal_received(int index) {
/* -save contents of tab with specified index */
    /*if ( editors->getEditor(index)->getFileName().isEmpty() ) {    //if no file is being edited
        on_actionSave_As_triggered();                           //perform a 'save as' instead of a 'save'
        return;                                                 //
    }
    editors->getEditor(index)->saveChanges();  //save changes of current editor
    QString file = editors->getEditor(index)->getOpenFilePath();
    if ( file.isEmpty() ) return;

    //editors->current()->saveChanges();  //save changes of current editor
    editors->getEditor(index)->writeToFile(file, true);*///save changes and mark file as saved
    saveFile( index );
}

void MainWindow::on_actionSave_As_triggered() {
/* -save content of tab to a new file */
    //editors->saveAs();
    /*QString file = QFileDialog::getSaveFileName(this, tr("Save As") );  //get a new file name
    if ( file.isEmpty() ) return;                                       //check if file name was specified
    QFile* file = new QFile(filePath);                                      //connect the new file
    file->open(QIODevice::WriteOnly | QIODevice::ReadWrite);                //open the new file in order to create it (in case it does not already exist)
    file->close();                                                         //
    //editors->current()->saveChangesTo(file);                                //save changes to the new file
    editors->current()->writeToFile(file);
    editors->current()->loadFile(file);*/
    saveFileAs( editors->currentIndex() );
}

void MainWindow::on_actionSave_Copy_As_triggered(){
/* -save a copy of the content in the tab to a new file */
    /*QString file = QFileDialog::getSaveFileName(this, tr("Save Copy As") ); //get a new file name
    if ( file.isEmpty() ) return;                                           //check if file name was specified
    QFile* file = new QFile(filePath);                                          //open the new file
    file->open(QIODevice::WriteOnly | QIODevice::ReadWrite);                    //open the new file in order to create it (in case it does not already exist)
    file->close();                                                              //
    //editors->current()->saveCopyOfChanges(file);                                //save changes to the new file
    editors->current()->writeToFile(file);*/
    saveFileCopyAs( editors->currentIndex() );
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
        /*if ( editors->getEditor(i)->getOpenFileName().isEmpty() ) {
        //if no file is currently open
            //perform a 'save as' instead of a 'save'
            QString file = QFileDialog::getSaveFileName(this, tr("Save Copy As") ); //get a new file name
            if ( file.isEmpty() ) return;                                           //check if file name was specified
            QFile* file = new QFile(filePath);                                          //open the new file
            file->open(QIODevice::WriteOnly | QIODevice::ReadWrite);                    //open the new file in order to create it (in case it does not already exist)
            file->close();                                                              //
            editors->getEditor(i)->writeToFile(file);                             //save changes to the new file
            editors->getEditor(i)->loadFile(file);
            continue;
        }
        editors->getEditor(i)->writeToFile( editors->getEditor(i)->getOpenFilePath(), true);*/   //save changes of current editor
        saveFile(i);
    }
}

void MainWindow::editTabChanged() {
/* -called when visible tab is changed to update lanugage selector menu */
    setLanguageSelectorMenu();
}



//~private method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::saveFile(int index) {
/* -save content to open file */
    QString file = editors->getEditor(index)->getOpenFilePath();
    if ( file.isEmpty() ) {                             //if no file is currently open, perform a 'save as' instead
        on_actionSave_As_triggered();
        return;
    }
    editors->getEditor(index)->writeToFile(file, true); //save changes and mark file as saved
}

void MainWindow::saveFileAs(int index) {
/* -save content to a new file and load it */
    QString file = QFileDialog::getSaveFileName(this, tr("Save As") );  //get a new file name
    if ( file.isEmpty() ) return;                                       //check if file name was actually specified
    editors->getEditor(index)->writeToFile(file);
    editors->getEditor(index)->loadFile(file);                          //open the newly created file
}

void MainWindow::saveFileCopyAs(int index) {
/* -save a copy of content to a new file (new file not loaded) */
    QString file = QFileDialog::getSaveFileName(this, tr("Save Copy As") );
    if ( file.isEmpty() ) return;
    editors->getEditor(index)->writeToFile(file);
}

void MainWindow::setLanguageSelectorMenu() {
/* -set the language selector menu from editor object */
    /*if (editors->count() <= 0) {    //if no editor windows are open
        langSelector = NULL;            //do not point to any language selectors
        return;                         //do nothing else
    }

    if (langSelector != NULL)               //if a language selector is being pointed to (the language selector from previous tab)
        ui->menuBar->removeAction(langSelector);*/ //stop pointing to it

    //langSelector = ui->menuBar->addMenu( editors->current()->languageSelector->languageMenu );  //set and point to the new languages selector
    ui->menuLanguage->menuAction()->setMenu( editors->current()->getLanguageMenu() );
}
