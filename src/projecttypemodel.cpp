/*
Project: Lepton Editor
File: projectypemodel.cpp
Author: Leonardo Banderali
Created: May 10, 2015
Last Modified: October 18, 2015

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
    /*QDir specsDir(LeptonConfig::mainSettings->getConfigDirPath("project_specs"));
    foreach (const QFileInfo& entry, specsDir.entryInfoList(QDir::NoDotAndDotDot |QDir::Files | QDir::Readable, QDir::Name)) {
        entries.append(new ItemEntry(entry.absoluteFilePath()));
    }*/
    /*ents.append(new QString("Str 1"));
    ents.append(new QString("Str 2"));*/
    //m = new ProjectTreeModel(_parent);
}

ProjectTypeModel::~ProjectTypeModel() {
    //qDeleteAll(entries);
    //qDeleteAll(ents);
    //delete m;
}



//~base class pure virtual functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QModelIndex ProjectTypeModel::index(int row, int column, const QModelIndex &parent) const {
    /*ItemEntry* p = (ItemEntry*)parent.internalPointer();

    ItemEntry* item;
    if (p != 0 && row < p->childCount())
        item = p->childAt(row);
    else if (p == 0 && row < entries.count())
        item = entries.at(row);
    else
        return createIndex(0, 0, (void*)0);

    return createIndex(row, column, (void*)item);*/
    /*if (parent.internalPointer() == 0 && row < ents.length()) {
        return createIndex(row, column, (void*)ents.at(row));
    } else
        return createIndex(0, 0, (void*)0);*/
    //m->index(row, column, parent);
}

QModelIndex ProjectTypeModel::parent(const QModelIndex &child) const {
    /*ItemEntry* item = (ItemEntry*)child.internalPointer();
    if (child.isValid() && item != 0) {
        ItemEntry* parent = item->parent();
        if (parent == 0)
            return createIndex(0, 0, (void*)0);
        else if (parent->parent() == 0)
            //return createIndex(entries.indexOf(parent), child.column(), parent);
            return createIndex(entries.indexOf(parent), 0, parent);
        else {
            ItemEntry* gp = parent->parent();
            //return createIndex(gp->indexOf(parent), child.column(), parent);
            return createIndex(gp->indexOf(parent), 0, parent);
        }
    } else
        return createIndex(0, 0, (void*)0);*/
    /*QString* s = (QString*)child.internalPointer();
    if (s != 0 && ents.contains(s)) {
        return createIndex(0, 0, (void*)0);
    } else
        return createIndex(0, 0, (void*)0);*/
    //m->parent(child);
}

int ProjectTypeModel::rowCount(const QModelIndex &parent) const {
    /*if (parent.internalPointer() == 0)
        return entries.count();
    else {
        ItemEntry* item = (ItemEntry*)(parent.internalPointer());
        return item->childCount();
    }*/
    /*if (parent.internalPointer() == 0) {
        return ents.count();
    } else
        return 0;*/
    //m->rowCount(parent);
}

int ProjectTypeModel::columnCount(const QModelIndex &parent) const {
    //return 1;   // one column for the project type, and one for the description
    //m->columnCount(parent);
}

QVariant ProjectTypeModel::data(const QModelIndex &index, int role) const {
    /*QVariant r;
    if (role == Qt::DisplayRole) {
        ItemEntry* item = (ItemEntry*)(index.internalPointer());
        if (index.column() == 0)
            r = QVariant(item->name());
        else if (index.column() == 1)
            r = QVariant(item->description());
    }
    return r;*/
    /*QString* s = (QString*)index.internalPointer();
    if (s != 0 && ents.contains(s) && role == Qt::DisplayRole) {
        return QVariant(*s);
    } else
        return QVariant();*/
    //m->data(index, role);
}



//~reimplemented virtual functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QVariant ProjectTypeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /*QVariant r;
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section == 0)
                r = QVariant("Project Type");
            else if (section == 1)
                r = QVariant("Description");
        }
    }
    return r;*/
    //return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags ProjectTypeModel::flags(const QModelIndex &index) const {
    /*ItemEntry* item = (ItemEntry*)index.internalPointer();
    if (item != 0)
        return item->flags();
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;*/
    //return QAbstractItemModel::flags(index);
}



//~other public functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
-returns the file path of a spec file specified by an index
*/
/*QString ProjectTypeModel::specFileFromIndex(const QModelIndex& index) {
    ItemEntry* item = (ItemEntry*)index.internalPointer();
    if (item != 0)
        return item->specFilePath();
    else
        return QString();
}*/



//~private class definitions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*ProjectTypeModel::ItemEntry::ItemEntry(const QString& filePath, ItemEntry* parent) :
itemName(), itemDescription(), itemFlags(Qt::NoItemFlags), specFile(filePath), itemParent(parent) {
    children.clear();
    if (specFile.exists()) {
        if (specFile.isDir()) {
            // if the file system item is a directory, use its name as project name, use no description, and get all its children
            QDir dir = specFile.dir();
            itemName = dir.dirName();
            itemDescription = "";
            QFileIconProvider iconProvider;
            itemIcon = iconProvider.icon(QFileIconProvider::Folder);
            itemFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            foreach(const QFileInfo& entry, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Readable, QDir::Name | QDir::DirsFirst)){
                children.append(new ItemEntry(entry.absoluteFilePath(), this));
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
                itemFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
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
}*/

