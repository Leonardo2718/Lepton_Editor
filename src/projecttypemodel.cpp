/*
Project: Lepton Editor
File: projectypemodel.cpp
Author: Leonardo Banderali
Created: May 10, 2015
Last Modified: May 11, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTypeModel class is a subclass of QAbstractItemModel that represents a collection
    of all project types known to Lepton.

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

#include "projecttypemodel.h"

// include Qt classes
#include <QVariant>
#include <QFileIconProvider>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

// include other project headers
#include "leptonconfig.h"



//~constructors and destructor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTypeModel::ProjectTypeModel(QObject* _parent) : QAbstractItemModel(_parent) {
    LeptonConfig::mainSettings->getConfigDirPath("project_specs");
}



//~base class pure virtual functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QModelIndex ProjectTypeModel::index(int row, int column, const QModelIndex &parent) const {
    ItemEntry* p = (ItemEntry*)parent.internalPointer();
    ItemEntry* item = p->childAt(row);
    return createIndex(row, column, (void*)item);
    //if (item != 0)
}

QModelIndex ProjectTypeModel::parent(const QModelIndex &child) const {
    ItemEntry* item = (ItemEntry*)child.internalPointer();
    if (child.isValid() && item != 0) {
        ItemEntry* parent = item->parent();
        if (parent->parent() == 0)
            return createIndex(entries.indexOf(parent), child.column(), parent);
        else {
            ItemEntry* gp = parent->parent();
            return createIndex(gp->indexOf(parent), child.column(), parent);
        }
    } else
        return createIndex(0, 0, (void*)0);
}

int ProjectTypeModel::rowCount(const QModelIndex &parent) const {
    if (parent.internalPointer() == 0)
        return entries.count();
    else {
        ItemEntry* item = (ItemEntry*)(parent.internalPointer());
        return item->childCount();
    }
}

int ProjectTypeModel::columnCount(const QModelIndex &parent) const {
    return 2;   // one column for the project type, and one for the description
}

QVariant ProjectTypeModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        ItemEntry* item = (ItemEntry*)(index.internalPointer());
        if (index.column() == 0)
            return QVariant(item->name());
        else if (index.column() == 1)
            return QVariant(item->description());
        else
            return QVariant();
    } else
        return QVariant();
}



//~reimplemented virtual functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QVariant ProjectTypeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section = 0)
                return QVariant("Project Type");
            else if (section = 1)
                return QVariant("Description");
        }
    }
}



//~private class definitions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ProjectTypeModel::ItemEntry::ItemEntry(const QString& filePath) : itemName(), itemDescription(), specFile(filePath) {
    if (specFile.exists()) {
        if (specFile.isDir()) {
            // if the file system item is a directory, use its name as project name, use no description, and get all its children
            QDir dir = specFile.dir();
            itemName = dir.dirName();
            itemDescription = "";
            QFileIconProvider iconProvider;
            itemIcon = iconProvider.icon(QFileIconProvider::Folder);
            foreach(const QString& dirItem, dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Readable, QDir::Name | QDir::DirsFirst)){
                children.append(new ItemEntry(dirItem));
            }
        } else if (specFile.isFile()) {
            // if the file system item is a file, parse it and get the display info from it.
            QJsonParseError err;
            QFile f(specFile.absoluteFilePath());
            f.open(QFile::ReadOnly);
            QJsonDocument specDoc = QJsonDocument::fromJson(f.readAll(), &err);
            f.close();

            if (err.error == QJsonParseError::NoError && specDoc.isObject()) {
                QJsonObject projectSpec = specDoc.object();
                itemName = projectSpec.value("project_type").toString();
                itemDescription = projectSpec.value("description").toString();
                if (projectSpec.contains("icon") && projectSpec.value("icon").toString() != "%NO_ICON") {
                    QString iconVal = projectSpec.value("icon").toString();
                    QDir specsDir = specFile.dir();
                    QString iconPath = specsDir.absoluteFilePath(iconVal);
                    if (QFileInfo(iconPath).exists()) {
                        QPixmap pmap(iconPath);
                        itemIcon = QIcon(pmap);
                    }
                }
            } else {
                itemName = filePath;
                itemDescription = "(File could not be parsed)";
            }
        } else {
            itemName = filePath;
            itemDescription = "(Unkown file system entry)";
        }
    } else {
        itemName = filePath;
        itemDescription = "(File does not exist)";
    }
}

ProjectTypeModel::ItemEntry::~ItemEntry() {
    qDeleteAll(children);
}

