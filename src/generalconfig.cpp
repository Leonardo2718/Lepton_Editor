/*
Project: Lepton Editor
File: generalconfig.cpp
Author: Leonardo Banderali
Created: May 18, 2014
Last Modified: October 15, 2014

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

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>

#include "generalconfig.h"

#include <QDebug>

//QDir GeneralConfig::configsDir = QDir(QString());

GeneralConfig::GeneralConfig(const QString& mainConfigFilePath) {
/* -get main config data from file */

    QFile configFile(mainConfigFilePath);               //get the config file
    configsDir.setPath( QFileInfo(mainConfigFilePath).absolutePath() );

    if ( configFile.exists() ) {                        //if the file actually exists

        configFile.open(QFile::ReadOnly);                           //open the file
        configData = configData.fromJson( configFile.readAll() );   //store the config data
        configFile.close();                                         //close the file

        if ( ! configData.isObject() ) {                            //if document is not value
            configData = configData.fromRawData("{\"NO_VALUE\": null}", 20);//set a default value
        }
    }
}


QVariant GeneralConfig::getValue(const QString& key, const QString& subKey_1, const QString &subKey_2) const {
/* -get the value that corresponds to `key` from the JSON config data object */

    QVariant returnValue;

    if ( ! subKey_2.isEmpty() ) {
        returnValue = configData.object().value(key).toObject().value(subKey_1).toObject().value(subKey_2).toVariant();
    }
    else if ( ! subKey_1.isEmpty() ) {
        returnValue = configData.object().value(key).toObject().value(subKey_1).toVariant();
    }
    else {
        returnValue = configData.object().value(key).toVariant();
    }

    return returnValue;
}


QColor GeneralConfig::getValueAsColor(const QString& key, const QString& subKey_1, const QString& subKey_2) const {
/*
    -get the value that corresponds to the given keys and return it as a color value
    -an empty color is returned if the value is not a valid color string
*/

    QString colorString = getValue(key, subKey_1, subKey_2).toString(); //get the configured value using the keys

    return getColorFromString( colorString );                           //convert the value to a color and return it
}


QFont GeneralConfig::getValueAsFont(const QString& key, const QString& subKey_1, const QString& subKey_2 ) const {
/*  -return a font based on the values from the keys */

    //define the return value
    QFont returnValue;

    //get a key-value list of the font properties
    QMap<QString, QVariant> fontProperties = getValue(key, subKey_1, subKey_2).toMap();

    //define a map of font styles for easy indexing using a string
    QMap<QString, QFont::Style> fontStyles;
    fontStyles["normal"] = QFont::StyleNormal;
    fontStyles["italic"] = QFont::StyleItalic;
    fontStyles["oblique"] = QFont::StyleOblique;

    //define a map of font weights for easy indexing using a string
    QMap<QString, QFont::Weight> fontWeights;//Light, Normal, DemiBold, Bold, Black
    fontWeights["light"] = QFont::Light;
    fontWeights["normal"] = QFont::Normal;
    fontWeights["demibold"] = QFont::DemiBold;
    fontWeights["bold"] = QFont::Bold;
    fontWeights["black"] = QFont::Black;

    //set the values for the font
    if ( fontProperties.contains("font_family") )
        returnValue.setFamily( fontProperties["font_family"].toString() );
    if ( fontProperties.contains("font_point_size") )
        returnValue.setPointSize( fontProperties["font_point_size"].toInt() );
    if ( fontProperties.contains("font_style") )
        returnValue.setStyle( fontStyles[ fontProperties["font_style"].toString() ] );
    if ( fontProperties.contains("font_weight") )
        returnValue.setWeight( fontWeights[ fontProperties["font_weight"].toString() ] );

    return returnValue;
}


QString GeneralConfig::getConfigDirPath(const QString& shortPath) {
/*
-returns absolute path to a sub-directory of the config directory
-takes into account that local config files override system files
-returns empty string if directory is not found
*/
#ifdef QT_DEBUG
    //check if file is in active directory
    QDir dir( QString("./config/").append(shortPath) );
    if ( dir.exists() ) return dir.absolutePath();

    //check if file is in parent directory
    dir = QDir( QString("../config/").append(shortPath) );
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
    QString userDirPath = configsDir.absolutePath().append("/").append(shortPath);
    QString systemDirPath  = QString("/etc/LeptonEditor/%1/").arg(shortPath);
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

QString GeneralConfig::getConfigData(const QString& filePath, const QString& item, const QString& field) {
/* -returns data from config file stored under 'item' and in 'field' */
    QFile configFile( filePath );
    if ( ! configFile.exists() ) return QString("");
    configFile.open(QIODevice::ReadOnly | QIODevice::ReadWrite);
    QTextStream configData(&configFile);
    QRegularExpressionValidator commentExp( QRegularExpression("^#.*") );
    QRegularExpressionValidator itemExp( QRegularExpression(QString("^\\[%1\\]").arg(item)) );
    QRegularExpressionValidator otherItemExp( QRegularExpression(QString("^\\[.*\\]")) );
    QRegularExpressionValidator fieldExp( QRegularExpression(QString("^%1:.*").arg(field)) );
    bool itemFound = false;
    int pos = 0;
    while ( ! configData.atEnd() ) {
        QString line = configData.readLine();
        if ( commentExp.validate(line,pos) == QValidator::Acceptable) continue;
        else if ( itemExp.validate(line,pos) == QValidator::Acceptable && itemFound == false ) itemFound = true;
        else if ( itemFound == true && otherItemExp.validate(line,pos) == QValidator::Acceptable) return QString();
        else if ( itemFound == true && fieldExp.validate(line,pos) == QValidator::Acceptable) {
            configFile.close();
            return line.remove(0, field.length() + 1).trimmed();
        }
    }
    configFile.close();
    return QString("");
}

QColor GeneralConfig::getColorFromString(QString colorString) {
/* -converts a color defined in a string to a 'QColor' object, using regexp validation, and returnes it */
    if ( colorString.isEmpty() ) return QColor();   //return if no color is specified

    //cleen up the string
    colorString = colorString.simplified();
    colorString = colorString.trimmed();

    QColor returnColor(0, 0, 0);    //dummy color to be returned
    int p = 0;                      //used for regexp validator

    //regexps to check color value
    QRegularExpressionValidator isRGB( QRegularExpression("\\d{1,3} \\d{1,3} \\d{1,3}") );              //check if RGB
    QRegularExpressionValidator isRGBA( QRegularExpression("\\d{1,3} \\d{1,3} \\d{1,3} \\d{1,3}") );    //check if RGBA
    QRegularExpressionValidator isHEX( QRegularExpression("^#([0-9A-Fa-f]{3}|[0-9A-Fa-f]{6}|[0-9A-Fa-f]{8}|[0-9A-Fa-f]{9}|[0-9A-Fa-f]{12})$") );//check if HEX code
    QRegularExpressionValidator isName( QRegularExpression("\\w+") );                                   //check if color name

    //check color value
    if ( isRGB.validate(colorString, p) == 2) {
        QStringList rgb = colorString.split( QRegularExpression("\\s") );
        returnColor.setRgb( rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt() );
    }
    else if ( isRGBA.validate(colorString, p) == 2) {
        QStringList rgb = colorString.split( QRegularExpression("\\s") );
        returnColor.setRgb( rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt(), rgb.at(3).toInt() );
    }
    else if ( isHEX.validate(colorString, p) == 2) {
        returnColor.setNamedColor(colorString);
    }
    else if ( isName.validate(colorString, p) ) {
        returnColor.setNamedColor( colorString.toLower().remove( QRegularExpression("\\s+") ) );
    }

    //check color is valid and return
    if ( returnColor.isValid() ) return returnColor;
    else return QColor();
}

QColor GeneralConfig::getDefaultPaper() {
/* -returns default 'QColor' for editor paper/background */

    //get data from config file, process it, and return value
    QColor c = getValueAsColor("theme_data", "paper_color");
    if ( c.isValid() ) return c;
    else return QColor(255,255,255);

}

QColor GeneralConfig::getDefaultTextColor() {
/* -returns default 'QColor' for editor text */
    //QString colorString = getConfigData( getConfigFilePath("config/theme.conf"), "Editor", "text-color");

    //process and return value
    //QColor c = getColorFromString(colorString);
    QColor c = getValueAsColor("theme_data", "text_color");
    if ( c.isValid() ) return c;
    else return QColor(0,0,0);
}

QFont GeneralConfig::getDefaultEditorFont() {
/* -returns default 'QFont' for editor text */
    QFont font = getValueAsFont("theme_data", "font");  //get values from config file
    return font;
}

QsciScintilla::WhitespaceVisibility GeneralConfig::getWhiteSpaceVisibility() {
/* -returns the visibility of white spaces in editor */

    //get data from config file
    QString s = getValue("theme_data", "whitespace_visibility").toString();

    //process and return value
    if (s == "visible") return QsciScintilla::WsVisible;
    else if (s == "invisible") return QsciScintilla::WsInvisible;
    else return QsciScintilla::WsInvisible;
}

QColor GeneralConfig::getWhiteSpaceColor() {
/* -returns color of white space when visible in editor */
    //get data from config file
    QString colorString = getConfigData( getConfigFilePath("config/theme.conf"), "Editor", "whitespace-color");

    //process and return value
    QColor c = getColorFromString(colorString);
    if ( c.isValid() ) return c;
    else return QColor(255,255,255);
}

QColor GeneralConfig::getMarginsBackground() {
/* -returns backgroung color for margins (line numbering etc.) */
    //get data from config file
    QString colorString = getConfigData( getConfigFilePath("config/theme.conf"), "Editor", "margins-background");

    //process and return value
    QColor c = getColorFromString(colorString);
    if ( c.isValid() ) return c;
    else return QColor(200,200,200);
}

QColor GeneralConfig::getMarginsForeground() {
/* -returns foreground color for margins (line numbering etc.) */
    //get data from config file
    QString colorString = getConfigData( getConfigFilePath("config/theme.conf"), "Editor", "margins-foreground");

    //process and return value
    QColor c = getColorFromString(colorString);
    //QColor c = getValueAsColor("theme_data", "paper_color");
    if ( c.isValid() ) return c;
    else return QColor(0,0,0);
}

void GeneralConfig::getStyleSheetInto(QString& styleSheet) {
/* -returns style sheet read from file */
    //QString path = getConfigData( getConfigFilePath("config/theme.conf"), "MainWindow", "theme").prepend("config/themes/");
    QString path = getValue("theme_data", "stylesheet_file").toString();
    path = configsDir.absolutePath().append("/").append(path);
    QFile file(path);
    if ( ! file.exists() ) {
        styleSheet = QString();
        return;
    }
    file.open(QIODevice::ReadOnly | QIODevice::ReadWrite);
    styleSheet = file.readAll().simplified();
    file.close();
}
