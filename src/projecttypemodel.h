/*
Project: Lepton Editor
File: projectypemodel.h
Author: Leonardo Banderali
Created: May 10, 2015
Last Modified: May 10, 2015

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

#include <QObject>



/*
This class models a collection of all project types (project specs) known to Lepton.
These are found int the `project_specs` config directory.
*/
class ProjectTypeModel : public QAbstractItemModel {
        Q_OBJECT

    public:
        // constructors and destructor
        ProjectTypeModel();
};

#endif // PROJECTTYPEMODEL_H
