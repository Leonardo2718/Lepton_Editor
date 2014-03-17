/*
Project: Lepton Editor
File: mainwindow.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: March 16, 2014

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

    /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //$$$ This file is only being used for testing purposes. $$$
          openFile("mainwindow.cpp");
          ui->menuBar
    //$$$                                                    $$$
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
}

MainWindow::~MainWindow() {
    delete ui;
}



//~private slot implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::on_actionOpen_triggered() {
/* -open a file in an editor tab */
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString());  //open pop-up window to prompt user for file to be opend
    if (!fileName.isEmpty()) openFile(fileName);    //if the file exists, open the file
}

void MainWindow::on_actionNew_triggered() {
/* -open a new tab */
    editors->addTab();
}

void MainWindow::on_actionSave_triggered(){
/* -save content of tab to corresponding file */
    editors->save();
}

void MainWindow::on_actionSave_As_triggered() {
/* -save content of tab to a new file */
    editors->saveAs();
}

void MainWindow::on_actionSave_Copy_As_triggered(){
/* -save a copy of the content in the tab to a new file */
    editors->saveCopyAs();
}

void MainWindow::on_actionProject_Manager_toggled(bool visible) {
/* -show/hide the project manager */
    ui->projectManager->setVisible(visible);
}

void MainWindow::on_actionEditor_Tools_toggled(bool visible) {
/* -show/hide the editor tools */
    ui->editorTools->setVisible(visible);
}

void MainWindow::editTabChanged() {
/* -called when visible tab is changed to update lanugage selector menu */
    setLanguageSelectorMenu();
}



//~private method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::openFile(QString fileName) {
/* -open a file in the current tab */
    /*QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }*/
    editors->openFile(fileName);
}

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
