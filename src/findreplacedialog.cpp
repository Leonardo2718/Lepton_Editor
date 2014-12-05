/*
Project: Lepton Editor
File: findreplacedialog.cpp
Author: Leonardo Banderali
Created: December 5, 2014
Last Modified: December 5, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of the `FindReplaceDialog` class.

Copyright (C) 2014 Leonardo Banderali

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


//include the header files for this class
#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"



//~public methodes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FindReplaceDialog::FindReplaceDialog(QWidget *parent) : QWidget(parent), ui(new Ui::FindReplaceDialog) {
    ui->setupUi(this);
}

FindReplaceDialog::~FindReplaceDialog() {
    delete ui;
}
