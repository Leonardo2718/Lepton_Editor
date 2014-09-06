/*
Project: Lepton Editor
File: generalconfig.h
Author: Leonardo Banderali
Created: May 18, 2014
Last Modified: September 6, 2014

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

//include Qt classes
#include <QString>
#include <QColor>
#include <QFont>
#include <QJsonDocument>

//include QScintilla classes
#include <Qsci/qsciscintilla.h>

class GeneralConfig {
    public:
        GeneralConfig(const QString& mainConfigFilePath);
        /* -get main config data from file */

        QVariant getValue(const QString& key, const QString& subKey_1 = 0, const QString& subKey_2 = 0) const;
        /*  -get the value that corresponds to `key` from the JSON config data object */

        QColor getValueAsColor(const QString& key, const QString& subKey_1 = 0, const QString& subKey_2 = 0) const;
        /*
            -get the value that corresponds to the given keys and return it as a color value
            -an empty color is returned if the value is not a valid color string
        */

        QFont getValueAsFont(const QString& key, const QString& subKey_1 = 0, const QString& subKey_2 = 0) const;
        /*  -return a font based on the values from the keys */

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

        static QString getConfigData(const QString &filePath, const QString& item, const QString& field);
        /* -returns data from config file 'filePath' stored under 'item' and in 'field' */

        static QColor getColorFromString(QString colorString);
        /* -converts a color defined in a string to a 'QColor' object, using regexp validation, and returnes it */

        static QColor getDefaultPaper();
        /* -returns default 'QColor' for editor paper/background */

        static QColor getDefaultTextColor();
        /* -returns default 'QColor' for editor text */

        static QFont getDefaultEditorFont();
        /* -returns default 'QFont' for editor text */

        QsciScintilla::WhitespaceVisibility getWhiteSpaceVisibility();
        /* -returns the visibility of white spaces in editor */

        static QColor getWhiteSpaceColor();
        /* -returns color of white space when visible in editor */

        static QColor getMarginsBackground();
        /* -returns backgroung color for margins (line numbering etc.) */

        static QColor getMarginsForeground();
        /* -returns foreground color for margins (line numbering etc.) */

        static void getStyleSheetInto(QString& styleSheet);
        /* -returns style sheet read from file */


    private:

        QJsonDocument configData;   //a JSON object to store the config data
};

#endif // GENERALCONFIG_H
