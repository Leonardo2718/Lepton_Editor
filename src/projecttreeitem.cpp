/*
Project: Lepton Editor
File: projecttreeitem.cpp
Author: Leonardo Banderali
Created: April 5, 2015
Last Modified: May 7, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTreeItem class represents a single item in the Lepton projects tree structure.

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

#include "projecttreeitem.h"

// include Qt classes
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QInputDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QPixmap>
#include <QIcon>

#include <QDebug>

//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTreeItem::ProjectTreeItem(const QVariantMap& _data, ProjectTreeItem* _parent) : QObject(_parent), parent(_parent), data(_data) {
    children.clear();
    contextMenuActions = new QActionGroup(0);
    load();
}

ProjectTreeItem::~ProjectTreeItem(){
    clear();
    delete contextMenuActions;
}



//~public getters and setters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const ProjectTreeItem* ProjectTreeItem::getParent() const {
    return parent;
}

/*
returns pointer to the child with a given index
*/
const ProjectTreeItem* ProjectTreeItem::getChild(int index) const {
    if (index < 0 || index >= children.length())
        return 0;
    else
        return children.at(index);
}

/*
returns internal index of a child
*/
int ProjectTreeItem::getChildIndex(const ProjectTreeItem* child) const {
    return children.indexOf((ProjectTreeItem*)child);
}

const QVariantMap& ProjectTreeItem::getData() const {
    return data;
}

/*
returns the datum associated with a given key
*/
QVariant ProjectTreeItem::getDataItem(const QString& key) const {
    return data.value(key, QVariant());
}



//~other public functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool ProjectTreeItem::hasChildren() const {
    return !children.isEmpty();
}

int ProjectTreeItem::childCount() const {
    return children.length();
}

const ProjectTreeItem* ProjectTreeItem::addChild(const QVariantMap& _data) {
    ProjectTreeItem* newChild = new ProjectTreeItem(_data, this);
    children.append(newChild);
    return newChild;
}

/*
removes a specific child
*/
bool ProjectTreeItem::removeChild(ProjectTreeItem* child) {
    return children.removeOne(child);
}

void ProjectTreeItem::addContextMenuAction(QAction* a) {
    contextMenuActions->addAction(a);
}

QActionGroup* ProjectTreeItem::getContextMenuActions() {
    return contextMenuActions;
}

/*
-loads this item by creating its children
*/
void ProjectTreeItem::load() {
    clear();

    if (data.contains("load_script")) {
        /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        %%% Will execute the load script specified in the spec file for this item. %%
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
    }

    QVariantMap projectSpec = data.value("project_spec").toMap();
    QString itemType = data.value("item_spec").toMap().value("type").toString();
    QString itemTypeKey;
    QString defaultContextMenuKey;

    if (data.value("is_directory").toBool()) {
        loadAsDir();
        defaultContextMenuKey = "default_dir_context_menu";
        itemTypeKey = "directory_types";
    } else if (data.value("is_file").toBool()) {
        defaultContextMenuKey = "default_file_context_menu";
        itemTypeKey = "file_types";
    }

    // load context menu actions
    QVariantMap contextMenuSpecs;
    if (data.value("type").toString() == "UNKNOWN_ITEM_TYPE")
        contextMenuSpecs = data.value("item_spec").toMap().value("context_menu").toMap();
    else
        contextMenuSpecs = projectSpec.value(itemTypeKey).toMap().value(itemType).toMap().value("context_menu").toMap();
    if (contextMenuSpecs.value("use_default").toBool())
        // add default context menu actions
        addContextActionsFor(this, projectSpec.value(defaultContextMenuKey).toMap());
    addContextActionsFor(this, contextMenuSpecs.value("actions").toMap());
}

/*
-reloads this item (currently does the same as `load()`)
*/
void ProjectTreeItem::reload() {
    load();
}



//~protected functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
removes all children
*/
void ProjectTreeItem::clear() {
    qDeleteAll(children);
    children.clear();
    foreach (QAction* a, contextMenuActions->actions()) {
        contextMenuActions->removeAction(a);
        delete a;
    }
}

/*
-loads the contents of a directory as part of this item
*/
void ProjectTreeItem::loadAsDir() {
    ProjectTreeItem* rootItem = this;
    QDir dir(data.value("path").toString());
    const QVariantMap dirSpec = data.value("item_spec").toMap();
    const QVariantMap projectSpec = data.value("project_spec").toMap();
    const QList<QVariant> parentDirTypeSpecs = data.value("parent_dir_type_specs").toList();
    const QList<QVariant> parentFileTypeSpecs = data.value("parent_file_type_specs").toList();

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
            if (isDir) {
                QVariantMap t = itemSpec;
                itemSpec.insert("unknown_directories", t);
                itemSpec.insert("unknown_file_types", t);
            }
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
            newData.insert("project_spec", projectSpec);
            newData.insert("path", entry.absoluteFilePath());
            newData.insert("project_file_spec", data.value("project_file_spec"));
            if (itemSpec.contains("load_script"))
                newData.insert("load_script", itemSpec.value("load_script"));
            QFileIconProvider iconProvider;
            QVariantMap itemTypeSpec = projectSpec.value(itemTypeKey).toMap().value(itemType).toMap();
            if (itemTypeSpec.contains("icon") && itemTypeSpec.value("icon").toString() != "%NO_ICON") {
                QString iconVal = itemTypeSpec.value("icon").toString();
                QDir specsDir = QFileInfo(data.value("project_file_spec").toString()).dir();
                QString iconPath = specsDir.absoluteFilePath(iconVal);
                if (QFileInfo(iconPath).exists()) {
                    QPixmap pmap(iconPath);
                    newData.insert("icon", QIcon(pmap));
                } else {
                    newData.insert("icon", QIcon());
                }
            } else if (isDir) {
                newData.insert("parent_dir_type_specs", directoryTypeSpecs);
                newData.insert("parent_file_type_specs", fileTypeSpecs);
                newData.insert("icon", iconProvider.icon(QFileIconProvider::Folder));
            } else if (isFile) {
                newData.insert("icon", iconProvider.icon(QFileIconProvider::File));
            } else
                newData.insert("icon", QIcon());
            rootItem->addChild(newData);

        }
    }
}

/*
-sets the context menu actions for `item` based on it's type
*/
void ProjectTreeItem::addContextActionsFor(ProjectTreeItem* item, const QVariantMap contextSpec) {
    foreach (const QString actionLabel, contextSpec.keys()) {
        QAction* a = new QAction(actionLabel, 0);
        a->setData(contextSpec.value(actionLabel));
        item->addContextMenuAction(a);
    }
}

/*
-return true if `itemName` matches the pattern
*/
bool ProjectTreeItem::itemNameMatches(const QString& itemName, const QString& pattern) {
    QRegularExpression regex(pattern);
    if (!regex.isValid()) return false;

    QRegularExpressionValidator validator(regex);

    int offset = 0;
    return validator.validate((QString&)itemName, offset) == QValidator::Acceptable;
}

