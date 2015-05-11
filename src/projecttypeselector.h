/*
Project: Lepton Editor
File: projecttypeselector.h
Author: Leonardo Banderali
Created: May 11, 2015
Last Modified: May 11, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The ProjectTypeSelector is a widget used for graphically selecting a project type
    from the project specs directory. The selected type can then be used to create a
    new project or open an existing one using the selected project spec.

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

#ifndef PROJECTTYPESELECTOR_H
#define PROJECTTYPESELECTOR_H

// include Qt classes
#include <QWidget>

// include other project headers
#include "projecttypemodel.h"

// define name spaces
namespace Ui {
class ProjectTypeSelector;
}



/*
The `ProjectTypeSelector` widget displays an interface for selecting a project type/spec. These are read from
the project specs directory and are presenting in a hierarchy following the directory's structure.
*/
class ProjectTypeSelector : public QWidget {
        Q_OBJECT

    public:
        // constructors and destructor
        explicit ProjectTypeSelector(QWidget *parent = 0);
        ~ProjectTypeSelector();

    private:
        Ui::ProjectTypeSelector *ui;
        ProjectTypeModel* projectTypes;
};

#endif // PROJECTTYPESELECTOR_H
