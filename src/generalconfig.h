/*
Project: Lepton Editor
File: generalconfig.h
Author: Leonardo Banderali
Created: May 18, 2014
Last Modified: May 18, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains a class definition with static members used to get and manipulate
    general information about the program (eg. path to local config files etc.).

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

#ifndef GENERALCONFIG_H
#define GENERALCONFIG_H

#include <QString>

class GeneralConfig {
    public:
        GeneralConfig();

        static QString getConfigDirPath(const QString& shortPath);
        /*
        -returns absolute path to a sub-directory of the config directory
        -takes into account that local config files override system files
        -returns empty string if directory is not found
        */

        static QString getConfigFilePath(const QString& shortPath);
        /*
        -returns absolute path to a config file specified using 'shortPath'
        -takes into account that local config files override system files
        -returns empty string if file is not found
        */

        static QString getLangsDirPath();
        /* -returns absolute path to language files directory */

        static QString getStylesDirPath();
        /* -returns absolute path to style files directory */

        static QString getLangFilePath(const QString& fileName);
        /* -returns absolute path to a language file */

        static QString getStyleFilePath(const QString& fileName);
        /* -returns absolute path to a styling file */
};

#endif // GENERALCONFIG_H
