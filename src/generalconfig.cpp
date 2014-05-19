/*
Project: Lepton Editor
File: generalconfig.cpp
Author: Leonardo Banderali
Created: May 18, 2014
Last Modified: May 18, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains a class implementaion with static members used to get and manipulate
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

#include <QFileInfo>
#include <QDir>

#include "generalconfig.h"

GeneralConfig::GeneralConfig() { }

QString GeneralConfig::getConfigDirPath(const QString& shortPath) {
/*
-returns absolute path to a sub-directory of the config directory
-takes into account that local config files override system files
-returns empty string if directory is not found
*/
#ifdef QT_DEBUG
    //check if file is in active directory
    QDir dir( QString("./").append(shortPath) );
    if ( dir.exists() ) return dir.absolutePath();

    //check if file is in parent directory
    dir.setCurrent( QString("../").append(shortPath) );
    if ( dir.exists() ) return dir.absolutePath();

    //if file was not found return nothing
    return QString();
#else

//this code was adapted from Notepadqq source code
#if defined(_WIN32)
    QString appData      = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString userDirPath = QString("%1/%2").arg(appData).arg(shortPath);
    QString systemDirPath  = QString("%1/%2").arg(qApp->applicationDirPath()).arg(shortPath);
#else
    QString userDirPath = QString("%1/%2/%3/%4").arg(QDir::homePath()).arg(".config").arg(qApp->applicationName().toLower()).arg(shortPath);
    QString systemDirPath  = QString("/usr/share/%1/%2").arg(qApp->applicationName().toLower()).arg(shortPath);
#endif
    if ( QDir(userDirPath).exists() ) return userDirPath;
    return systemDirPath;

#endif
}

QString GeneralConfig::getConfigFilePath(const QString& shortPath) {
/*
-returns absolute path to a config file specified using 'shortPath'
-takes into account that local config files override system files
-returns empty string if file is not found
*/
#ifdef QT_DEBUG
    //check if file is in active directory
    QFileInfo file( QString("./").append(shortPath) );
    if ( file.exists() ) return file.absoluteFilePath();

    //check if file is in parent directory
    file.setFile( QString("../").append(shortPath) );
    if ( file.exists() ) return file.absoluteFilePath();

    //if file was not found return nothing
    return QString();
#else

//this code was adapted from Notepadqq source code
#if defined(_WIN32)
    QString appData      = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString userFilePath = QString("%1/%2").arg(appData).arg(shortPath);
    QString systemFilePath  = QString("%1/%2").arg(qApp->applicationDirPath()).arg(shortPath);
#else
    QString userFilePath = QString("%1/%2/%3/%4").arg(QDir::homePath()).arg(".config").arg(qApp->applicationName().toLower()).arg(shortPath);
    QString systemFilePath  = QString("/usr/share/%1/%2").arg(qApp->applicationName().toLower()).arg(shortPath);
#endif
    if ( QFile(userFilePath).exists() ) return userFilePath;
    return systemFilePath;

#endif
}

QString GeneralConfig::getLangsDirPath() {
/* -returns absolute path to language files directory */
    return getConfigDirPath("languages");
}

QString GeneralConfig::getStylesDirPath() {
/* -returns absolute path to style files directory */
    return getConfigDirPath("styles");
}

QString GeneralConfig::getLangFilePath(const QString& fileName) {
/* -returns absolute path to a language file */
    return getLangsDirPath().append("/").append(fileName);
}

QString GeneralConfig::getStyleFilePath(const QString& fileName) {
/* -returns absolute path to a styling file */
    return getStylesDirPath().append("/").append(fileName);
}
