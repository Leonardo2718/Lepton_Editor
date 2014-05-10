/*
Project: Lepton Editor
File: leptonlexer.cpp
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: May 9, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of the LeptonLexer class which is used to perform highlighting
    instead of the QSyntaxHighlighter class.  It is subclassed fromt the QsciLexerCustom class which
    is part of QScintilla.

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

#include <QFile>
#include <QRegularExpressionValidator>
#include <Qsci/qsciscintilla.h>

#include "leptonlexer.h"

#include <QDebug>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonLexer::LeptonLexer(QObject *parent) : QsciLexerCustom(parent) {
    languageName = NULL;
    getStyleFormat("styles/default.xml");
    //setDefaultPaper( QColor(200, 200, 200) );
    /*setColor( QColor(0, 150, 0), 0);
    setPaper( QColor(255, 255, 255), 1);*/
    //this->setAutoIndentStyle(QsciScintilla::AiMaintain);
}

const char* LeptonLexer::language() const {
/*
-returnes language name
-returns 'NULL' if not language is set
*/
    return languageName;
}

QString LeptonLexer::description(int style) const {
/* -returnes description of a style */
    return "No description yet";
}

void LeptonLexer::styleText(int start, int end) {
/* -should be called whenever text must be highilighted */
    //qDebug() << "Got called: start=" << start << " end=" << end;
    /*startStyling(0, 0);
    setStyling(end, 0);*/
    aplyStyleTo(0, end, 0);
}

void LeptonLexer::aplyStyleTo(int start, int end, int style) {
/* -applies 'style' between positions 'start' and 'end' inclusively */
    startStyling(start, 0);
    setStyling(end, style);
}

bool LeptonLexer::getStyleFormat(const QString& styleFilePath) {
/*
-gets style info from file
-returns 'true' if the style information was successfully extracted, false otherwise
*/
    //get the xml file
    QFile xmlFile(styleFilePath);
    QDomDocument styleDoc("style_doc");
    if ( ! styleDoc.setContent(&xmlFile) ) return false;

    //get the document element
    QDomElement styleElement = styleDoc.documentElement();
    if ( styleElement.nodeName() != "format") return false;

    setDefaultPaper( QColor(200, 200, 200) );

    //get data from the different elements with no 'type' attribute
    getStyleData( styleElement.lastChildElement("numbers"), NUMBER_STYLE);
    getStyleData( styleElement.lastChildElement("linecomment"), LINECOMMENT_STYLE);
    getStyleData( styleElement.lastChildElement("blockcomment"), BLOCKCOMMENT_STYLE);
    getStyleData( styleElement.lastChildElement("quotations"), QUOTE_STYLE);
    getStyleData( styleElement.lastChildElement("escapedchar"), ESCAPECHAR_STYLE);

    //get data from the different elements with 'type' attribute
    getTypedStyleElementData( styleElement.elementsByTagName("keywords"), KEYWORD_STYLE_MIN, KEYWORD_STYLE_MAX );
    getTypedStyleElementData( styleElement.elementsByTagName("expression"), EXPRESSION_STYLE_MIN, EXPRESSION_STYLE_MAX ) ;
    getTypedStyleElementData( styleElement.elementsByTagName("lineexpression"), LINEEXP_STYLE_MIN, LINEEXP_STYLE_MAX );
    getTypedStyleElementData( styleElement.elementsByTagName("blockexpression"), BLOCKEXP_STYLE_MIN, BLOCKEXP_STYLE_MAX );

    //set paper for all styles
    for (int i = 0; i < 32; i++) {
        setPaper( defaultPaper(), i);
    }

    return true;
}

bool LeptonLexer::getTypedStyleElementData(const QDomNodeList& styleNodeList, StyleTypeValue min, StyleTypeValue max) {
/*
-retrieves data from elements with the same name but which are differentiated by the value of the 'type' attribute
 (eg. keywords, expression, lineexpression, blockexpression)
-parameters:
    styleNodeList: list of all the nodes with the same name
    min: lowest style number for the item (used to bound the value of 'type') (eg. 5 for keywords)
    max: highest style number for the item (used to bound the value of 'type') (eg 14 for keywords)
*/
    for (int i = 0, len = styleNodeList.length(); i < len; i++) {
        if ( styleNodeList.at(i).isNull() ) continue;

        QDomElement styleElement = styleNodeList.at(i).toElement();  //convert node to element

        if ( styleElement.attribute("type").isEmpty() ) continue;    //extract the 'type' id number
        quint8 typeNumber = styleElement.attribute("type").toInt();

        if ( min + typeNumber > max) continue;   //if 'typeNumber' is outside the range of allocated style numbers, continue to next node
        getStyleData(styleElement, min + typeNumber);
    }
    return true;
}

void LeptonLexer::getStyleData(QDomElement styleElement, quint8 style) {
/*
-extracts style info from 'styleElement'
-if more than one styling item is defined (eg. multiple 'color' tags used), the last item wins
*/
    if ( ! styleElement.lastChildElement("color").isNull() ){
        QString colorString = styleElement.lastChildElement("color").text();
        QColor color = getColor(colorString);
        setColor(color, style);
    }
}

QColor LeptonLexer::getColor(QString colorString) {
/* -converts color defined in a string to a 'QColor' object and returnes it */
    if ( colorString.isEmpty() ) return QColor();   //return if no color is specified

    //cleen up the string
    colorString = colorString.simplified();
    colorString = colorString.trimmed();

    QColor returnColor(0, 0, 0);    //dummy color to be returned
    int p = 0;                      //used for regexp validator
    //QString testStr("#bbbb2222BBBB");   //=> Debug

    //regexps to check color value
    QRegularExpressionValidator isRGB( QRegularExpression("\\d{1,3} \\d{1,3} \\d{1,3}") );              //check if RGB
    QRegularExpressionValidator isRGBA( QRegularExpression("\\d{1,3} \\d{1,3} \\d{1,3} \\d{1,3}") );    //check if RGBA
    QRegularExpressionValidator isHEX( QRegularExpression("^#([0-9A-Fa-f]{3}|[0-9A-Fa-f]{6}|[0-9A-Fa-f]{8}|[0-9A-Fa-f]{9}|[0-9A-Fa-f]{12})$") );//check if HEX code
    QRegularExpressionValidator isName( QRegularExpression("\\w+") ); //check if color name

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
    //qDebug() << QRegularExpression("^#([0-9A-Fa-f]{3}|[0-9A-Fa-f]{6}|[0-9A-Fa-f]{8}|[0-9A-Fa-f]{9}|[0-9A-Fa-f]{12})$").match(testStr);
    //qDebug() << isHEX.validate(testStr,p);
    //qDebug() << returnColor.name();

    if ( returnColor.isValid() ) return returnColor;
    else return QColor(0, 0, 0);
}
