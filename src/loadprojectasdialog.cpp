/*
Project: Lepton Editor
File: loadprojectasdialog.cpp
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

#include "loadprojectasdialog.h"
#include "ui_loadprojectasdialog.h"

// inlucde Qt classes
#include <QVBoxLayout>



LoadProjectAsDialog::LoadProjectAsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LoadProjectAsDialog) {
    ui->setupUi(this);
    specSelector = new ProjectTypeSelector(ui->selectArea);
    QVBoxLayout* l = new QVBoxLayout(ui->selectArea);
    l->addWidget(specSelector);
    ui->selectArea->setLayout(l);
}

LoadProjectAsDialog::~LoadProjectAsDialog() {
    delete specSelector;
    delete ui;
}
