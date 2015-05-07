/*
Project: Lepton Editor
File: leptonproject.cpp
Author: Leonardo Banderali
Created: March 15, 2015
Last Modified: May 6, 2015

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

    // load the new project
    load();
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
void LeptonProject::load() {
    // set project data
    data.insert("name", workingDirectory.dirName());
    data.insert("type", projectSpec.value("project_type").toString());
    data.insert("is_directory", true);
    data.insert("is_file", false);
    data.insert("path", workingDirectory.absolutePath());
    data.insert("item_spec", projectSpec.value("working_directory"));
    data.insert("project_spec", projectSpec);
    QFileIconProvider iconProvider;
    data.insert("icon", iconProvider.icon(QFileIconProvider::Folder));

    // clear and load the project
    clear();
    addContextActionsFor(this, projectSpec.value("project_context_menu").toMap());
    loadAsDir();
}



//~private methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

