/*
Project: Lepton Editor
File: leptonconfig.h
Author: Leonardo Banderali
Created: September 4, 2014
Last Modified: September 6, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file defines a namespace for objects which hold configuration data and settings.
    This includes data read from config files.

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

#ifndef LEPTONCONFIG_H
#define LEPTONCONFIG_H

//include Qt classes
#include <QDir>

//include other Lepton files needed
#include "generalconfig.h"



namespace LeptonConfig {
/* -a namespace to hold all the objects that define config data and settings */

//get data from the main config file
#ifdef QT_DEBUG
static GeneralConfig mainSettings("./config/config.json");
#else
static GeneralConfig mainSettings(QDir::homePath().append("./Lepton_Editor/config.json"));
#endif

}

#endif // LEPTONCONFIG_H
