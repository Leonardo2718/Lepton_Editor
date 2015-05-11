/*
Project: Lepton Editor
File: loadprojectasdialog.h
Author: Leonardo Banderali
Created: May 11, 2015
Last Modified: May 11, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    The `LoadProjectAsDialog` is a dialog window to select a project spec.

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

#ifndef LOADPROJECTASDIALOG_H
#define LOADPROJECTASDIALOG_H

// include Qt classes
#include <QDialog>

// include other poject headers
#include "projecttypeselector.h"

// define namespaces
namespace Ui {
class LoadProjectAsDialog;
}

/*
The `LoadProjectAsDialog` dialog shows a window with a `ProjectTypeSelector` and lets the user select
a project spec that will be used to load a project.
*/
class LoadProjectAsDialog : public QDialog {
        Q_OBJECT

    public:
        // constructors and destructor
        explicit LoadProjectAsDialog(QWidget *parent = 0);
        ~LoadProjectAsDialog();

    private:
        Ui::LoadProjectAsDialog *ui;
        ProjectTypeSelector* specSelector;
};

#endif // LOADPROJECTASDIALOG_H
