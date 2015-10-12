/*
Project: Lepton Editor
File: mainwindow.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: October 11, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of the main window object.

Copyright (C) 2015 Leonardo Banderali

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
#include <QSettings>
#include <QVariant>
#include <QLabel>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "leptonconfig.h"

#include <QDebug>

//~public method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
/*
-class constructor builds the main window
*/
    //setup UI elements
    ui->setupUi(this);  //reference the main window

    selectorSpaceTab = new QActionGroup(this);
    selectorSpaceTab->addAction( ui->actionUse_Tabs );
    selectorSpaceTab->addAction( ui->actionUse_Spaces );

    //set session object information
    QSettings::setDefaultFormat(QSettings::NativeFormat);   //%%% I may decide to change this later on and use my own format
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, LeptonConfig::mainSettings->getConfigDirPath("sessions"));

    //setup main window
    setWindowTitle("Lepton Editor");

    statusLabelTemplate = LeptonConfig::mainSettings->getValue("status_bar", "template").toString();
    statusLabel = new QLabel();
    ui->statusBar->addPermanentWidget(statusLabel);

    //setup other windows
    configsEditor.setParent(this, Qt::Dialog);
    findReplace.setParent(this, Qt::Dialog);

    //instantiate editing area
    editors = new EditorTabBar(this);
    ui->editorArea->layout()->addWidget(editors);

    //create a new editor
    //editors->addTab();
    insertTab();

    setLanguageSelectorMenu();  //set language selector from editing tab instance

    projectView = new QTreeView(this);
    ui->projectManagerArea->layout()->addWidget(projectView);
    //projectListModel = new ProjectTreeModel();
    projectListModel = new ProjectListModel{};
    projectView->setModel(projectListModel);
    projectView->setContextMenuPolicy(Qt::CustomContextMenu);

    //connect signals to appropriate slots
    connect(projectView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(projectItemContextMenuRequested(QPoint)) );
    //connect(projectListModel, SIGNAL(openFileRequest(QString)), this, SLOT(openFileRequested(QString)));
    //connect(projectView, SIGNAL(doubleClicked(QModelIndex)), projectListModel, SLOT(itemDoubleClicked(QModelIndex)));
    connect(editors, SIGNAL(currentChanged(int)), this, SLOT(editTabChanged()) );
    connect(editors, SIGNAL(saveSignal(int)), this, SLOT(save_signal_received(int)) );
    connect(selectorSpaceTab, SIGNAL(triggered(QAction*)), this, SLOT(changeSpaceTabUse(QAction*)) );
    connect(&findReplace, SIGNAL(findClicked(FindReplaceDialog::DialogParameters)), this, SLOT(findInCurrent(FindReplaceDialog::DialogParameters)) );
    connect(&findReplace, SIGNAL(findNextClicked(FindReplaceDialog::DialogParameters)), this, SLOT(findNextInCurrent()) );
    connect(&findReplace, SIGNAL(replaceClicked(FindReplaceDialog::DialogParameters)), this, SLOT(replaceInCurrent(FindReplaceDialog::DialogParameters)) );

    QString styleSheet;
    LeptonConfig::mainSettings->getStyleSheetInto(styleSheet);
    qApp->setStyleSheet(styleSheet);

    //load previous session
    loadSession();
}

MainWindow::~MainWindow() {
    //delete projectListModel;
    delete projectListModel;
    delete statusLabel;
    delete ui;
}



//~protected member implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::closeEvent(QCloseEvent *event) {
/* -called whenever a close window is requested */
    saveSession();
    disconnect(editors, SIGNAL(currentChanged(int)), this, SLOT(editTabChanged()) );
    int err = editors->closeAll();  //request to close all open tabs
    if (err != 0) event->ignore();  //if a tab was not closed, do not close the window
    else event->accept();           //if all tabs where closed, close the main window
}



//~private slot implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::on_actionOpen_File_triggered() {
/* -open a file in an editor tab */
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), getDialogDirPath() );    //open pop-up window to prompt user for file to be opend
    openFile(filePath);
}

void MainWindow::on_actionNew_File_triggered() {
/* -open a new tab */
    int tabIndex = editors->addTab();
    ui->menuBar->insertMenu( ui->menuSettings->menuAction(), editors->getEditor(tabIndex)->getLanguageMenu() );
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
/* -called when visible tab is changed to update main window */
    if (editors->count() > 0) {
        setLanguageSelectorMenu();
        setSpaceTabSelector();
        editors->current()->setFocus(Qt::TabFocusReason);
        updateStatusLabel();
    }
}

void MainWindow::on_actionOpen_Project_triggered() {
/* -called to add a new project directory to tree model */
    if ( !ui->projectManagerArea->isVisible() ) ui->actionProject_Manager->trigger();
    //projectListModel->openProjectRequest();
    projectListModel->openProject();
}

void MainWindow::on_actionNew_Project_triggered() {
/* -called to create and add a new project to tree model */
    if ( !ui->projectManagerArea->isVisible() ) ui->actionProject_Manager->trigger();
    //projectListModel->newProjectRequest();
}

void MainWindow::projectItemContextMenuRequested(const QPoint& position) {
/* -called when project item is right-clicked */
    /*QModelIndex itemIndex = projectView->indexAt(position);             //get index of clicked item
    if ( ! itemIndex.isValid() ) return;
    QMenu* menu = new QMenu(projectView);                               //create menu to be displayed
    QList< QAction* > actions = projectListModel->getActionsFor(itemIndex);  //get context menu actions for item
    menu->addActions(actions);
    menu->move( projectView->viewport()->mapToGlobal(position) );       //move the menu to an appropriat location
    menu->show();*/
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

void MainWindow::on_actionSettings_Editor_triggered() {
/*  -opens the settings editor window */
    configsEditor.show();
}

void MainWindow::changeSpaceTabUse(QAction* actionTriggered) {
/*  -sets the current editor tab to use tabs or spaces, depending on `actionTriggered` */
    if ( actionTriggered->text() == ui->actionUse_Spaces->text() ) editors->current()->setIndentationsUseTabs(false);
    else if ( actionTriggered->text() == ui->actionUse_Tabs->text() ) editors->current()->setIndentationsUseTabs(true);
}

void MainWindow::on_actionReplace_Tabs_with_Spaces_triggered() {
/*  -called to replace tabs with spaces */
    editors->current()->changeTabsToSpaces();
}

void MainWindow::on_actionReplace_Spaces_with_Tabs_triggered(){
/*  -called to replace spaces with tabs */
    editors->current()->changeSpacesToTabs();
}

void MainWindow::on_action_Find_Replace_triggered(){
/*  -called to show the Find/Replace dialog */
    findReplace.show();
}

void MainWindow::findInCurrent(const FindReplaceDialog::DialogParameters& parameters) {
/*  -called to perform a "find" on the current tab file using the data in `parameters` */
    editors->current()->findFirst(parameters.findText, parameters.isRegex, parameters.caseSensitive,
                                  parameters.matchWholeWord, parameters.wrap, parameters.forwardSearch);
}

void MainWindow::findNextInCurrent() {
/*  -called to perform a "find next" on the current tab file (should be called after `findInCurrent()` ) */
    editors->current()->findNext();
}

void MainWindow::replaceInCurrent(const FindReplaceDialog::DialogParameters& parameters) {
/*  -called to perform a "replace" on the current tab file using the data in `parameters` */
    editors->current()->replace(parameters.replaceText);
}

/*
remove trailing spaces from the current file
*/
void MainWindow::on_actionRemove_trailing_spaces_triggered(){
    editors->current()->removeTrailingSpaces();
}

/*
update the status bar label
*/
void MainWindow::updateStatusLabel() {
    int line = 0;
    int col = 0;
    editors->current()->getCursorPosition(&line, &col);
    QString labelText = statusLabelTemplate;
    labelText.replace(QRegularExpression("%l"), tr("%0").arg(line + 1));
    labelText.replace(QRegularExpression("%c"), tr("%0").arg(col));
    labelText.replace(QRegularExpression("%C"), tr("%0").arg(col + 1));
    statusLabel->setText(labelText);
}



//~private method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MainWindow::openFile(const QString& filePath) {
/* -opens a specified file in an editor tab */
    if ( filePath.isEmpty() ) return;
    if ( (editors->count() < 1) || (! editors->current()->text().isEmpty()) ) { //if text is already presend in the current editor, create a new tab
        //qint8 i = editors->addTab();
        int i = insertTab();
        editors->setCurrentIndex(i);
    }
    editors->current()->loadFile(filePath);         //insert text into editor
}

void MainWindow::saveFile(int index) {
/* -save content to open file */
    QString file = editors->getEditor(index)->getOpenFilePath();

    if ( file.isEmpty() ) {                                     // if no file is currently open, perform a 'save as' instead
        on_actionSave_As_triggered();
        return;
    }

    ScintillaEditor* currentEditor = editors->getEditor(index);
    if (ui->actionRemove_trailing_spaces_on_save->isChecked())  // remove trailing spaces if required
        currentEditor->removeTrailingSpaces();
    currentEditor->writeToFile(file, true);         // save changes and mark file as saved
}

void MainWindow::saveFileAs(int index) {
/* -save content to a new file and load it */
    QString file = QFileDialog::getSaveFileName(this, tr("Save As"), getDialogDirPath() );  // get a new file name
    if ( file.isEmpty() ) return;                                                           // check if file name was actually specified

    ScintillaEditor* currentEditor = editors->getEditor(index);
    if (ui->actionRemove_trailing_spaces_on_save->isChecked())  // remove trailing spaces if required
        currentEditor->removeTrailingSpaces();
    currentEditor->writeToFile(file);
    currentEditor->loadFile(file);                  // open the newly created file
}

void MainWindow::saveFileCopyAs(int index) {
/* -save a copy of content to a new file (new file not loaded) */

    QString file = QFileDialog::getSaveFileName(this, tr("Save Copy As"), getDialogDirPath() );
    if ( file.isEmpty() ) return;

    ScintillaEditor* currentEditor = editors->getEditor(index);
    if (ui->actionRemove_trailing_spaces_on_save->isChecked())
        currentEditor->removeTrailingSpaces();
    currentEditor->writeToFile(file);
}

void MainWindow::setLanguageSelectorMenu() {
/* -set the language selector menu from editor object */
    if ( editors->count() < 1 ) return;
    ui->menuLanguage->menuAction()->setMenu( editors->current()->getLanguageMenu() );
}

void MainWindow::setSpaceTabSelector() {
/*  -set the space/tab selector to match the state of the current editor tab */
    if ( editors->count() < 1) return;

    if ( editors->current()->indentationsUseTabs() ) ui->actionUse_Tabs->setChecked(true);
    else ui->actionUse_Spaces->setChecked(true);
}

void MainWindow::loadSession() {
/* -load settings and configs from saved session */
    projectListModel->loadSession();

    QSettings session;  // session object

    // load layout settings
    if ( session.value("windowMaximized").toBool() ) this->showMaximized();
    else this->resize( session.value("windowWidth").toInt(), session.value("windowHeight").toInt() );

    ui->projectManagerArea->setVisible( session.value("managerVisible", false).toBool() );
    ui->projectManagerArea->resize( session.value("managerWidth").toInt(), session.value("managerHeight").toInt() );
    ui->actionProject_Manager->setChecked( session.value("managerVisible", false).toBool() );

    ui->editorTools->setVisible( session.value("toolsVisible", false).toBool() );
    ui->editorTools->resize( session.value("toolsWidth").toInt(), session.value("toolsHeight").toInt() );
    ui->actionEditor_Tools->setChecked( session.value("toolsVisible", false).toBool() );

    // load editor settings
    ui->actionRemove_trailing_spaces_on_save->setChecked(session.value("removeTrailingSpacesOnSave",false).toBool());

    // load previously opened files
    QList< QVariant > fileList = session.value("listOfOpenFiles").toList();
    for (int i = 0, c = fileList.count(); i < c; i++) {
        this->openFile( fileList.at(i).toString() );
    }
}

void MainWindow::saveSession() {
/* -save settings and configs of this session */
    projectListModel->saveSession();

    QSettings session;  // session object

    // save layout settings
    if ( this->isMaximized() ) session.setValue("windowMaximized", true);
    else {
        session.setValue("windowWidth", this->width() );
        session.setValue("windowHeight", this->height() );
    }

    session.setValue("managerVisible", ui->projectManagerArea->isVisible() );
    session.setValue("managerWidth", ui->projectManagerArea->width() );
    session.setValue("managerHeight", ui->projectManagerArea->height() );

    session.setValue("toolsVisible", ui->editorTools->isVisible() );
    session.setValue("toolsWidth", ui->editorTools->width() );
    session.setValue("toolsHeight", ui->editorTools->height() );

    // save editor settings
    session.setValue("removeTrailingSpacesOnSave", ui->actionRemove_trailing_spaces_on_save->isChecked());

    // save opened files
    QList< QVariant > fileList;
    for (int i = 0, c = editors->count(); i < c; i++) {
        fileList.append( editors->getEditor(i)->getOpenFilePath() );
    }
    session.setValue("listOfOpenFiles", fileList);
}

QString MainWindow::getDialogDirPath(const QString& location) {
/*
    -gets path to a directory for dialogs to open
    -returns directory of file open in current editor tab when `location = 0` and when a file is actually open
    -returns home directory (or executable's directory if `QT_DEBUG` is defined) when no file is open or `location = "home"`
*/
    QString path;

    if (location == "home" || (! editors->current()->isFileOpen()) ) {
#ifdef QT_DEBUG
        path = QDir::currentPath(); //use the executable's directory
#else
        path = QDir::homePath();    //use the home directory
#endif
    }
    else {
        path = editors->current()->getOpenFileDir();    //get the directory of the file being edited in the current tab
    }

    return path;
}

/*
inserts a new editor tab and returns the new editor object
*/
int MainWindow::insertTab() {
    int index = editors->addTab();
    ScintillaEditor* newEditor = dynamic_cast<ScintillaEditor*>(editors->widget(index));
    connect(newEditor, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(updateStatusLabel()));
    return index;
}
