/*
Project: Lepton Editor
File: leptonproject.h
Author: Leonardo Banderali
Created: March 15, 2015
Last Modified: March 15, 2015

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

#ifndef LEPTONPROJECT_H
#define LEPTONPROJECT_H

#include <QObject>

class LeptonProject : public QObject {
        Q_OBJECT

    public:
        explicit LeptonProject(QObject *parent = 0);
        ~LeptonProject();

    signals:

    public slots:
};

#endif // LEPTONPROJECT_H
