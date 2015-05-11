/*
Project: Lepton Editor
File: projectypemodel.h
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

#ifndef PROJECTTYPEMODEL_H
#define PROJECTTYPEMODEL_H

// include Qt classes
#include <QObject>
#include <QAbstractItemModel>
#include <QFileInfo>
#include <QIcon>



/*
This class models a collection of all project types (project specs) known to Lepton.
These are found int the `project_specs` config directory.
*/
class ProjectTypeModel : public QAbstractItemModel {
        Q_OBJECT

    public:
        // constructors and destructor
        ProjectTypeModel(QObject* _parent = 0);

        // base class pure virtual functions
        QModelIndex index(int row, int column, const QModelIndex &parent) const;
        QModelIndex parent(const QModelIndex &child) const;
        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;

        // reimplemented virtual functions
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        Qt::ItemFlag flags(const QModelIndex &index) const;

        // other public functions

        QString specFileFromIndex(const QModelIndex& index);
        /*  -returns the file path of a spec file specified by an index */

    private:

        /*
        `ItemEntry` is a convenience class representing an item in the model.
        */
        class ItemEntry {
            public:
                // constructors and destructor
                ItemEntry(const QString& filePath);
                ~ItemEntry();

                // getters and setters
                QString name() const {
                    return itemName;
                }

                QString description() const {
                    return itemDescription;
                }

                QIcon getIcon() const {
                    return itemIcon;
                }

                Qt::ItemFlag flags() const {
                    return itemFlags;
                }

                QString specFilePath() const {
                    return specFile.absoluteFilePath();
                }

                int childCount() const {
                    return children.count();
                }

                ItemEntry* childAt(int i) {
                    if (i < 0 || i > children.count())
                        return 0;
                    else
                        return children.at(i);
                }

                int indexOf(const ItemEntry* child) const {
                    return children.indexOf((ItemEntry*)child);
                }

                ItemEntry* parent() {
                    return itemParent;
                }



            private:
                QString itemName;
                QString itemDescription;
                QIcon itemIcon;
                Qt::ItemFlag itemFlags;
                QFileInfo specFile;
                QList<ItemEntry*> children;
                ItemEntry* itemParent;
        };

        QList<ItemEntry*> entries;
};

#endif // PROJECTTYPEMODEL_H
