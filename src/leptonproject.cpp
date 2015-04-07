/*
Project: Lepton Editor
File: leptonproject.cpp
Author: Leonardo Banderali
Created: March 15, 2015
Last Modified: April 6, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The LeptonProject class represents a project being edited in Lepton.

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

#include "leptonproject.h"

// include other project headers
#include "leptonconfig.h"

// include Qt classes
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QRegularExpressionValidator>
#include <QFileDialog>
#include <QInputDialog>
#include <QFileIconProvider>


#include <QDebug>
//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonProject::LeptonProject(ProjectTreeItem* _parent, const QString& projectDir, const QString& specPath)
: ProjectTreeItem(QVariantMap(), _parent), workingDirectory(projectDir) {
    // if the project directory does not exist, create it
    if (!workingDirectory.exists())
        workingDirectory.mkdir(workingDirectory.absolutePath());

    // if a project spec file was specified load it, other wise load the default one
    if (specPath.isEmpty())
        specFilePath = LeptonConfig::mainSettings->getConfigDirPath("project_specs").append("/simplecpp.json");
    else
        specFilePath = specPath;
    loadSpec(specFilePath);

    // set project data
    data.insert("name", workingDirectory.dirName());
    data.insert("type", projectSpec.value("project_type").toString());
    data.insert("is_directory", true);
    data.insert("is_file", false);
    QFileIconProvider iconProvider;
    data.insert("icon", iconProvider.icon(QFileIconProvider::Folder));

    // load the new project
    reloadProject();

    // connect signals to slots
    //connect(contextMenuActions, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuActionTriggered(QAction*)));
    //connect(this, SIGNAL(refreshProject()), this, SLOT(loadItem()));
}

LeptonProject::~LeptonProject() {
}



//~getters and setters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LeptonProject::setName(const QString& newName) {

    /*########################################################################################
    ### In order to rename the project, the QDir object muse `cd` to the parent directory,  ##
    ### change the name of the project directory, and `cd` back into the renamed directory. ##
    ########################################################################################*/

    if(workingDirectory.cdUp()) {   // cd into the parent directory
        workingDirectory.rename(data.value("name").toString(), newName);    // rename the project directory
        workingDirectory.cd(newName);                                       // cd back into the renamed directory
        data.insert("name", newName);
    }
}

/* -returns path to the project's spec file */
const QString& LeptonProject::getSpecFilePath() {
    return specFilePath;
}

/*QList<QAction*> LeptonProject::getActions() {
    return contextMenuActions->actions();
}*/



//~other public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-loads the project specification from a file
*/
void LeptonProject::loadSpec(const QString& filePath) {
    QJsonParseError err;
    QFile f(filePath);
    f.open(QFile::ReadOnly);
    QJsonDocument specDoc = QJsonDocument::fromJson(f.readAll(), &err);
    f.close();

    if (err.error == QJsonParseError::NoError && specDoc.isObject()) {
        projectSpec = specDoc.object().toVariantMap();
        specFilePath = filePath;
    }
}

//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-load the contents of the project
*/
void LeptonProject::reloadProject() {
    ProjectTreeItem* p = this;
    while(p->getParent() != 0)
        p = (ProjectTreeItem*)p->getParent();
    emit p->changingItem(this);
    //emit qModel->layoutAboutToBeChanged();
    clear();
    loadDir(this, workingDirectory, projectSpec.value("working_directory").toMap());
    //addContextActionsFor(this, projectSpec.value("project_context_menu").toMap());
    QVariantMap contextSpec = projectSpec.value("project_context_menu").toMap();
    foreach (const QString actionLabel, contextSpec.keys()) {
        QAction* a = new QAction(actionLabel, 0);
        a->setData(contextSpec.value(actionLabel));
        //contextMenuActions->addAction(a);
        addContextMenuAction(a);
    }
    emit p->itemChanged();
    //emit qModel->layoutChanged();
}

void LeptonProject::contextMenuActionTriggered(QAction* actionTriggered) {
    QString actionData = actionTriggered->data().toString();

    ProjectTreeItem* p = this;
    while(p->getParent() != 0)
        p = (ProjectTreeItem*)p->getParent();

    if (actionData == "%ADD_FILE"){
        QString fileName = QFileDialog::getSaveFileName(0, "New File", workingDirectory.absolutePath());
        if (!fileName.isEmpty()) {
            QFile newFile(fileName);
            newFile.open(QFile::ReadWrite);
            newFile.close();
            reloadProject();
        }
    } else if (actionData == "%ADD_DIRECTORY") {
        QString dirName = QFileDialog::getSaveFileName(0, "New Directory", workingDirectory.absolutePath(), QString(),0,QFileDialog::ShowDirsOnly);
        if (!dirName.isEmpty()) {
            workingDirectory.mkpath(dirName);
            reloadProject();
        }
    } else if (actionData == "%REFRESH_PROJECT") {
        reloadProject();
    } else if (actionData == "%RENAME_PROJECT") {
        QString newName = QInputDialog::getText(0, "Rename Project", tr("Change project name from \"%0\" to:").arg(data.value("name").toString()));
        if (!newName.isEmpty()) {
            setName(newName);
            reloadProject();
        }
    } else if (actionData == "%CLOSE_PROJECT") {
        emit p->removingItem(this);
        ProjectTreeItem* p = (ProjectTreeItem*)this->getParent();
        bool r = p->removedChild(this);
        if(r) {
            emit p->itemRemoved();
            delete this;
        } else {
            emit p->itemRemoved();
        }
    } else {
    }
}



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-loads the contents of a directory that is part of the project
*/
void LeptonProject::loadDir(ProjectTreeItem* rootItem, QDir dir, const QVariantMap& dirSpec,
                            const QList<QVariant>& parentDirTypeSpecs, const QList<QVariant>& parentFileTypeSpecs ) {

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden  | QDir::System | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);

    QList<QVariant> templateDirSpecs = dirSpec.value("template_directories").toList();
    QList<QVariant> directoryTypeSpecs = dirSpec.value("directory_types").toList();
    directoryTypeSpecs.append(parentDirTypeSpecs);

    QList<QVariant> templateFileSpecs = dirSpec.value("template_files").toList();
    QList<QVariant> fileTypeSpecs = dirSpec.value("file_types").toList();
    fileTypeSpecs.append(parentFileTypeSpecs);

    foreach (const QFileInfo& entry, entries) {
        // define variables for later reference
        QString entryName;
        QList<QVariant> templateSpecs;
        QList<QVariant> typeSpecs;
        QString itemTypeKey;
        QString unknownTypes;
        QVariantMap itemSpec;
        bool itemMatched = false;
        bool isDir = entry.isDir();
        bool isFile = entry.isFile();

        // initiallize reference variables, depending on the type of the filesystem entry
        if (isDir) {
            entryName = QDir(entry.absoluteFilePath()).dirName();
            templateSpecs = templateDirSpecs;
            typeSpecs = directoryTypeSpecs;
            itemTypeKey = "directory_types";
            unknownTypes = "unknown_file_types";
        } else if (isFile) {
            entryName = entry.fileName();
            templateSpecs = templateFileSpecs;
            typeSpecs = fileTypeSpecs;
            itemTypeKey = "file_types";
            unknownTypes = "unknown_directories";
        } else {
            continue;
        }

        // check if the entry matches a project item
        foreach (const QVariant& specV, templateSpecs) {
            QVariantMap spec = specV.toMap();
            if (itemNameMatches(entryName, spec.value("name").toString())) {
                itemSpec = spec;
                itemMatched = true;
                break;
            }
        }

        // if the entry has not yet been matched, continue checking if the entry matches a project item
        if (!itemMatched) {
            foreach (const QVariant& specV, typeSpecs) {
                QVariantMap spec = specV.toMap();
                if (itemNameMatches(entryName, projectSpec.value(itemTypeKey).toMap().value(spec.value("type").toString()).toMap().value("name_filter").toString())) {
                    itemSpec = spec;
                    itemMatched = true;
                    break;
                }
            }
        }

        QString itemType = itemSpec.value("type").toString();   // store the type of the item

        if (!itemMatched && dirSpec.value(unknownTypes).toMap().value("are_visible").toBool()) {
            itemType = "UNKNOWN_ITEM_TYPE";
            itemSpec = dirSpec.value(unknownTypes).toMap();
            itemMatched = true;
        }

        if (itemMatched) {
            // if the item was matched, add it to the project normally

            QVariantMap newData;
            newData.insert("name", entryName);
            newData.insert("type", itemType);
            newData.insert("is_directory", isDir);
            newData.insert("is_file", isFile);
            newData.insert("item_spec", itemSpec);
            newData.insert("path", entry.absoluteFilePath());
            QFileIconProvider iconProvider;
            if (isDir)
                newData.insert("icon", iconProvider.icon(QFileIconProvider::Folder));
            else if (isFile)
                newData.insert("icon", iconProvider.icon(QFileIconProvider::File));
            else
                newData.insert("icon", QIcon());
            ProjectTreeItem* newItem = (ProjectTreeItem*)rootItem->addChild(newData);
            //LeptonProjectItem* newItem = (LeptonProjectItem*)rootItem->addChild(entryName, itemType);
            QVariantMap contextMenuSpecs = projectSpec.value(itemTypeKey).toMap().value(itemType).toMap().value("context_menu").toMap();

            if (isDir) {
                loadDir(newItem, QDir(entry.absoluteFilePath()), itemSpec, directoryTypeSpecs, fileTypeSpecs);  // load project items in the sub directory
                if (contextMenuSpecs.value("use_default").toBool()) {
                    // add default context menu actions for the item if specified in the spec file
                    addContextActionsFor(newItem, projectSpec.value("default_dir_context_menu").toMap());
                }
            } else if (isFile) {
                if (contextMenuSpecs.value("use_default").toBool()) {
                    addContextActionsFor(newItem, projectSpec.value("default_file_context_menu").toMap());
                }
            }

            // add context menu actions for the item
            addContextActionsFor(newItem, contextMenuSpecs.value("actions").toMap());

        }
    }
}

/*
-return true if `itemName` matches the pattern
*/
bool LeptonProject::itemNameMatches(const QString& itemName, const QString& pattern) {
    QRegularExpression regex(pattern);
    if (!regex.isValid()) return false;

    QRegularExpressionValidator validator(regex);

    int offset = 0;
    return validator.validate((QString&)itemName, offset) == QValidator::Acceptable;
}

/*
-sets the context menu actions for `item` based on it's type
*/
void LeptonProject::addContextActionsFor(ProjectTreeItem* item, const QVariantMap contextSpec) {
    foreach (const QString actionLabel, contextSpec.keys()) {
        QAction* a = new QAction(actionLabel, 0);
        a->setData(contextSpec.value(actionLabel));
        item->addContextMenuAction(a);
    }
}

