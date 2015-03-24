/*
Project: Lepton Editor
File: leptonproject.cpp
Author: Leonardo Banderali
Created: March 15, 2015
Last Modified: March 23, 2015

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


//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonProject::LeptonProject(const QString& projectDir, const QString& specPath) : LeptonProjectItem(), workingDirectory(projectDir) {
    // if the project directory does not exist, create it
    if (!workingDirectory.exists())
        workingDirectory.mkdir(workingDirectory.absolutePath());

    // if a project spec file was specified load it, other wise load the default one
    if (specPath.isEmpty())
        specFilePath = LeptonConfig::mainSettings->getConfigDirPath("project_specs").append("/simplecpp.json");
    else
        specFilePath = specPath;
    loadSpec(specFilePath);

    // initialize project data
    name = workingDirectory.dirName();
    type = projectSpec.value("project_type").toString();
    projectParent = 0;

    // load the new project
    loadProject();
}

LeptonProject::LeptonProject(const QDir& projectDir, const QString& specPath) : LeptonProjectItem(), workingDirectory(projectDir) {
    // if the project directory does not exist, create it
    if (!workingDirectory.exists())
        workingDirectory.mkdir(workingDirectory.absolutePath());

    // if a project spec file was specified load it, other wise load the default one
    if (specPath.isEmpty())
        specFilePath = LeptonConfig::mainSettings->getConfigDirPath("project_specs").append("/simplecpp.json");
    else
        specFilePath = specPath;
    loadSpec(specFilePath);

    // initialize project data
    name = workingDirectory.dirName();
    type = projectSpec.value("project_type").toString();
    projectParent = 0;

    // load the new project
    loadProject();
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
        workingDirectory.rename(name, newName); // rename the project directory
        workingDirectory.cd(newName);           // cd back into the renamed directory
        name = newName;
    }
}

/* -returns path to the project's spec file */
const QString& LeptonProject::getSpecFilePath() {
    return specFilePath;
}



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

/*
-load the contents of the project
*/
void LeptonProject::loadProject() {
    clear();
    loadDir(this, workingDirectory, projectSpec.value("working_directory").toMap());
}



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-loads the contents of a directory that is part of the project
*/
void LeptonProject::loadDir(LeptonProjectItem* rootItem, QDir dir, const QVariantMap& dirSpec,
                            const QList<QVariant>& parentDirTypeSpecs, const QList<QVariant>& parentFileTypeSpecs ) {

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden  | QDir::System | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);

    QList<QVariant> templateDirSpecs = dirSpec.value("template_directories").toList();
    QList<QVariant> directoryTypeSpecs = dirSpec.value("directory_types").toList();
    directoryTypeSpecs.append(parentDirTypeSpecs);

    QList<QVariant> templateFileSpecs = dirSpec.value("template_files").toList();
    QList<QVariant> fileTypeSpecs = dirSpec.value("file_types").toList();
    fileTypeSpecs.append(parentFileTypeSpecs);

    foreach (const QFileInfo& entry, entries) {
        QString entryName;
        QList<QVariant> templateSpecs;
        QList<QVariant> typeSpecs;
        QString itemTypeKey;
        QString unknownTypes;
        QVariantMap itemSpec;
        bool itemMatched = false;

        if (entry.isDir()) {
            entryName = QDir(entry.absoluteFilePath()).dirName();
            templateSpecs = templateDirSpecs;
            typeSpecs = directoryTypeSpecs;
            itemTypeKey = "directory_types";
            unknownTypes = "unknown_file_types";
        } else if (entry.isFile()) {
            entryName = entry.fileName();
            templateSpecs = templateFileSpecs;
            typeSpecs = fileTypeSpecs;
            itemTypeKey = "file_types";
            unknownTypes = "unknown_directories";
        } else {
            continue;
        }

        foreach (const QVariant& specV, templateSpecs) {
            QVariantMap spec = specV.toMap();
            if (itemNameMatches(entryName, spec.value("name").toString())) {
                itemSpec = spec;
                itemMatched = true;
                break;
            }
        }

        if (!itemMatched) {
            foreach (const QVariant& specV, typeSpecs) {
                QVariantMap spec = specV.toMap();
                if (itemNameMatches(entryName, projectSpec.value(itemTypeKey).toMap().value(spec.value("type").toString()).toString())) {
                    itemSpec = spec;
                    itemMatched = true;
                    break;
                }
            }
        }

        if (itemMatched) {
            LeptonProjectItem* newItem = (LeptonProjectItem*)rootItem->addChild(entryName, itemSpec.value("type").toString());
            if (entry.isDir())
                loadDir(newItem, QDir(entry.absoluteFilePath()), itemSpec, directoryTypeSpecs, fileTypeSpecs);
        } else if (dirSpec.value(unknownTypes).toMap().value("are_visible").toBool()) {
            rootItem->addChild(entryName, "UNKNOWN_ITEM_TYPE");
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

