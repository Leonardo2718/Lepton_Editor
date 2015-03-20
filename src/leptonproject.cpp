/*
Project: Lepton Editor
File: leptonproject.cpp
Author: Leonardo Banderali
Created: March 15, 2015
Last Modified: March 19, 2015

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

// include Qt classes
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QRegularExpressionValidator>

// include other project classes
#include "leptonconfig.h"


//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonProject::LeptonProject(const QString& projectDir, const QString& specFilePath) : QObject(0), LeptonProjectItem(), workingDirectory(projectDir) {
    // if the project directory does not exist, create it
    if (!workingDirectory.exists())
        workingDirectory.mkdir(workingDirectory.absolutePath());

    // if a project spec file was specified load it, other wise load the default one
    if (specFilePath.isEmpty())
        loadSpec(LeptonConfig::mainSettings->getConfigDirPath("project_specs").append("/simplecpp.json"));
    else
        loadSpec(specFilePath);

    // initialize project data
    name = workingDirectory.dirName();
    type = projectSpec.value("project_type").toString();
    projectParent = 0;

    // load the new project
    loadProject();
}

LeptonProject::LeptonProject(const QDir& projectDir, const QString& specFilePath) : QObject(0), workingDirectory(projectDir) {
    // if the project directory does not exist, create it
    if (!workingDirectory.exists())
        workingDirectory.mkdir(workingDirectory.absolutePath());

    // if a project spec file was specified load it, other wise load the default one
    if (specFilePath.isEmpty())
        loadSpec(LeptonConfig::mainSettings->getConfigDirPath("project_specs").append("/simplecpp.json"));
    else
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

    if (err.error == QJsonParseError::NoError && specDoc.isObject())
        projectSpec = specDoc.object().toVariantMap();
}



//~public slots~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-load the contents of the project
*/
void LeptonProject::loadProject() {
    loadDir(workingDirectory, projectSpec.value("working_directory").toMap());
}



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-loads the contents of a directory that is part of the project
*/
void LeptonProject::loadDir(QDir dir, QVariantMap dirSpec, QList<QVariant> parentDirTypeSpecs, QList<QVariant> parentFileTypeSpecs ) {

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden  | QDir::System | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);

    QList<QVariant> templateDirSpecs = dirSpec.values("template_directory");
    QList<QVariant> directoryTypeSpecs = dirSpec.values("directory_type");
    directoryTypeSpecs.append(parentDirTypeSpecs);

    QList<QVariant> templateFileSpecs = dirSpec.values("template_file");
    QList<QVariant> fileTypeSpecs = dirSpec.values("file_type");
    fileTypeSpecs.append(parentFileTypeSpecs);

    foreach (const QFileInfo& entry, entries) {
        if (entry.isDir()) {
            bool dirMatched = false;

            foreach (const QVariant& specV, templateDirSpecs) {
                QVariantMap spec = specV.toMap();
                if (addItemIfMatched(entry, this,
                                     spec.value("name").toString(),
                                     spec.value("type").toString())) {
                    dirMatched = true;
                    loadDir(entry.dir(), spec, directoryTypeSpecs, fileTypeSpecs);
                    break;
                }
            }

            if (!dirMatched) {
                foreach (const QVariant& specV, templateDirSpecs) {
                    QVariantMap spec = specV.toMap();
                    if (addItemIfMatched(entry, this,
                                         projectSpec.value("directory_types").toMap().value(spec.value("type").toString()).toString(),
                                         spec.value("type").toString())) {
                        dirMatched = true;
                        loadDir(entry.dir(), spec, directoryTypeSpecs, fileTypeSpecs);
                        break;
                    }
                }
            }

            if (!dirMatched && dirSpec.value("unknown_directories").toMap().value("are_visible").toBool())
                this->addChild(entry.fileName(), "UNKNOWN_TYPE");

        } else if (entry.isFile()) {
            bool fileMatched = false;

            foreach (const QVariant& specV, templateFileSpecs) {
                QVariantMap spec = specV.toMap();
                if (addItemIfMatched(entry, this,
                                     spec.value("name").toString(),
                                     spec.value("type").toString())) {
                    fileMatched = true;
                    break;
                }
            }

            if (!fileMatched) {
                foreach (const QVariant& specV, templateFileSpecs) {
                    QVariantMap spec = specV.toMap();
                    if (addItemIfMatched(entry, this,
                                         projectSpec.value("file_types").toMap().value(spec.value("type").toString()).toString(),
                                         spec.value("type").toString())) {
                        fileMatched = true;
                        break;
                    }
                }
            }

            if (!fileMatched && dirSpec.value("unknown_file_types").toMap().value("are_visible").toBool())
                this->addChild(entry.fileName(), "UNKNOWN_TYPE");
        }
    }
}

/*
    -if the file system item `fsItem`s name matches `pattern`, it is added to the project item `item`
    -returns true if the file was added, false if not
*/
bool LeptonProject::addItemIfMatched(const QFileInfo& fsItem, LeptonProjectItem* item, const QString& pattern, const QString& type) {
    QRegularExpression regex(pattern);
    if (!regex.isValid()) return false;
    bool rval = false;

    QRegularExpressionValidator validator(regex);
    QString name;
    if (fsItem.isFile())
        name = fsItem.fileName();
    else if (fsItem.isDir())
        name = fsItem.dir().dirName();

    int offset = 0;
    if (validator.validate(name, offset) == QValidator::Acceptable) {
        if (fsItem.isFile())
            item->addChild(fsItem.fileName(), type);
        else if (fsItem.isDir())
            item->addChild(fsItem.dir().dirName(), type);
        rval = true;
    } else
        rval = false;

    return rval;
}

