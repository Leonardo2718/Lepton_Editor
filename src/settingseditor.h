/*
Project: Lepton Editor
File: settingseditor.h
Author: Leonardo Banderali
Created: October 17, 2014
Last Modified: October 17, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file defines the `SettingsEditor` class.  This class provides a GUI for editing
    values in Lepton's config files.

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

#ifndef SETTINGSEDITOR_H
#define SETTINGSEDITOR_H

//include Qt classes
#include <QWidget>

class SettingsEditor : public QWidget
{
    Q_OBJECT
    public:
        explicit SettingsEditor(QWidget *parent = 0);
};

#endif // SETTINGSEDITOR_H
