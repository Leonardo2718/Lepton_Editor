/*
Project: Lepton Editor
File: mainwindow.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: September 3, 2014

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
#include <QCloseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "generalconfig.h"


#include <QDebug>


//~public method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
/*
-class constructor builds the main window
*/
    ui->setupUi(this);  //reference the main window

    //hide the extra featuers
    ui->projectManagerArea->hide();
    ui->editorTools->hide();

    //instantiate editing area
    editors = new EditorTabBar(this);
    ui->editorArea->layout()->addWidget(editors);

    //create a new editor
    editors->addTab();

    setLanguageSelectorMenu();  //set language selector from editing tab instance

    projectList = new QTreeView(this);
    ui->projectManagerArea->layout()->addWidget(projectList);
    projectListModel = new ProjectModel();
    projectList->setModel(projectListModel);
    projectList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(projectList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(projectItemContextMenuRequested(QPoint)) );
    connect(projectListModel, SIGNAL(openFileRequested(QString)), this, SLOT(openFileRequested(QString)) );
    connect(editors, SIGNAL(currentChanged(int)), this, SLOT(editTabChanged()) );
    connect(editors, SIGNAL(saveSignal(int)), this, SLOT(save_signal_received(int)) );
    connect(projectList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openFileFromProjecManager(QModelIndex)) );

    QString styleSheet;
    GeneralConfig::getStyleSheetInto(styleSheet);
    qApp->setStyleSheet(styleSheet);
}

MainWindow::~MainWindow() {
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

void MainWindow::on_actionOpen_File_triggered() {
/* -open a file in an editor tab */
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"));  //open pop-up window to prompt user for file to be opend
    openFile(filePath);
}

void MainWindow::on_actionNew_File_triggered() {
/* -open a new tab */
    int tabIndex = editors->addTab();
    ui->menuBar->insertMenu( ui->menuCustomize->menuAction(), editors->getEditor(tabIndex)->getLanguageMenu() );
    setLanguageSelectorMenu();
}

void MainWindow::on_actionSave_triggered() {
/* -save content of tab to corresponding file */
    saveFile( editors->currentIndex() );
}

void MainWindow::save_signal_received(int index) {
/* -save contents of tab with specified index */
    saveFile( index );
}

void MainWindow::on_actionSave_As_triggered() {
/* -save content of tab to a new file */
    saveFileAs( editors->currentIndex() );
}

void MainWindow::on_actionSave_Copy_As_triggered(){
/* -save a copy of the content in the tab to a new file */
    saveFileCopyAs( editors->currentIndex() );
}

void MainWindow::on_actionProject_Manager_toggled(bool visible) {
/* -show/hide the project manager */
    ui->projectManagerArea->setVisible(visible);
}

void MainWindow::on_actionEditor_Tools_toggled(bool visible) {
/* -show/hide the editor tools */
    ui->editorTools->setVisible(visible);
}

void MainWindow::on_actionSave_All_triggered() {
/* -save changes to all documents */
    for (int i = 0, l = editors->count(); i < l; i++) {
        saveFile(i);
    }
}

void MainWindow::editTabChanged() {
/* -called when visible tab is changed to update lanugage selector menu */
    setLanguageSelectorMenu();
}

void MainWindow::on_actionOpen_Project_triggered() {
/* -called to add a new project directory to tree model */
    if ( !ui->projectManagerArea->isVisible() ) ui->actionProject_Manager->trigger();
    projectListModel->openProjectRequest();
}

void MainWindow::on_actionNew_Project_triggered() {
/* -called to create and add a new project to tree model */
    if ( !ui->projectManagerArea->isVisible() ) ui->actionProject_Manager->trigger();
    projectListModel->newProjectRequest();
}

void MainWindow::projectItemContextMenuRequested(const QPoint& position) {
/* -called when project item is right-clicked */
    QModelIndex itemIndex = projectList->indexAt(position);                 //get index of clicked item
    if ( ! itemIndex.isValid() ) return;
    QMenu* menu = new QMenu(projectList);                                   //create menu to be displayed
    QList< QAction* > actions = projectListModel->getActionsFor(itemIndex); //get context menu actions for item
    menu->addActions(actions);
    menu->move( projectList->viewport()->mapToGlobal(position) );           //move the menu to an appropriat location
    menu->show();
}

void MainWindow::openFileRequested(const QString& filePath) {
/* -called when an object (ex. project list) requests to open a file */
    openFile(filePath);
}

void MainWindow::on_actionGitHub_Page_triggered() {
/* -called to open the Lepton Editor GitHub page in the user's default web browser */
    QDesktopServices::openUrl( QUrl("https://github.com/Leonardo2718/Lepton_Editor") );  //open the Lepton Editor GitHub page the user's default web browser
}

void MainWindow::on_actionAbout_Lepton_Editor_triggered() {
/* -called to display an "about window" for Lepton */
    QMessageBox::about(this, "About - Lepton Editor",
                      "<p>Lepton Editor is a text editor oriented towards programmers.  It's intended to be a "
                      "flexible and extensible code editor which developers can easily customize to their liking.</p> "
                      "<p>Copyright &copy; 2014 Leonardo Banderali</p>"
                      "<p>Lepton Editor is free software: you can redistribute it and/or modify "
                      "it under the terms of the GNU General Public License as published by "
                      "the Free Software Foundation, either version 3 of the License, or "
                      "any later version.</p> "
                      "<p>Lepton Editor is distributed in the hope that it will be useful, "
                      "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                      "GNU General Public License for more details.</p>"
                      "<p>You should have received a copy of the GNU General Public License "
                      "along with this program.  If not, see <a href='http://www.gnu.org/licenses/'> "
                      "http://www.gnu.org/licenses/<a>.</p> "
                      );
}

void MainWindow::on_actionAbout_Qt_triggered(){
/* -called to display an "about window" for Qt */
    QMessageBox::aboutQt(this, "About Qt - LeptonEditor");
}

void MainWindow::openFileFromProjecManager(QModelIndex index) {
/* -opens a file when it is double clicked in the project manager list */
    if ( index.isValid() ) {                            //if the index provided is valid
        ProjectItem* item = (ProjectItem*)index.internalPointer();  //get the model item
        if ( item->isFile() ) {                                     //if the item is a file
            QString filePath = item->getPath();                         //get the path to the file
            openFile(filePath);                                         //open the file
        }
    }
}



//~private method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::openFile(const QString& filePath) {
/* -opens a specified file in an editor tab */
    if ( filePath.isEmpty() ) return;
    if ( (editors->count() < 1) || (! editors->current()->text().isEmpty()) ) { //if text is already presend in the current editor, create a new tab
        qint8 i = editors->addTab();
        editors->setCurrentIndex(i);
    }
    editors->current()->loadFile(filePath);         //insert text into editor
}

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
    if ( editors->count() < 1 ) return;
    ui->menuLanguage->menuAction()->setMenu( editors->current()->getLanguageMenu() );
}
