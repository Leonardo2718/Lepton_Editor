/*
Project: Lepton Editor
File: leptonlexer.cpp
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: May 12, 2014

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
#include <QFileInfo>
#include <QDir>
#include <QRegularExpressionValidator>
#include <QTextStream>
#include <Qsci/qsciscintilla.h>

#include "leptonlexer.h"

#include <QDebug>



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonLexer::LeptonLexer(QObject *parent) : QsciLexerCustom(parent) {
    languageName = NULL;

    resetRules();

    //getLanguageData("languages/cplusplus.xml");
    getLanguageData("languages/python.xml");
    //getStyleFormat("styles/default.xml");
    //this->setAutoIndentStyle(QsciScintilla::AiMaintain);
}

const char* LeptonLexer::language() const {
/*
-returnes language name
-returns 'NULL' if no language is set
*/
    return languageName;
}

QString LeptonLexer::description(int style) const {
/* -returnes description of a style */

    //check for extreme cases first
    if (style == 32) return QString("Style #32 is the default style.");
    else if (style < 0 || style > 32) return tr("Style #%1 is not defined.").arg(style);

    QString returnStr;          //dummy string to store return value
    returnStr = tr("Style #%1 is the style for ").arg(style);

    //append value
    if (style == NUMBER_STYLE) {
        returnStr += tr("numbers.");
    }
    else if (style == QUOTE_STYLE) {
        returnStr += tr("quotes.");
    }
    else if (style >= KEYWORD_STYLE_MIN && style <= KEYWORD_STYLE_MAX) {
        returnStr += tr("keywords of type %1.").arg(style - KEYWORD_STYLE_MIN);
    }
    else if (style >= EXPRESSION_STYLE_MIN && style <= EXPRESSION_STYLE_MAX) {
        returnStr += tr("expressions of type %1.").arg(style - EXPRESSION_STYLE_MIN);
    }
    else if (style == LINECOMMENT_STYLE) {
        returnStr += tr("line comments.");
    }
    else if (style >= LINEEXP_STYLE_MIN && style <= LINEEXP_STYLE_MAX) {
        returnStr += tr("line expressions of type %1.").arg(style - LINEEXP_STYLE_MIN);
    }
    else if (style == BLOCKCOMMENT_STYLE) {
        returnStr += tr("block comments.");
    }
    else if (style >= BLOCKEXP_STYLE_MIN && style <= BLOCKEXP_STYLE_MAX) {
        returnStr += tr("block expressions of type %1.").arg(style - BLOCKEXP_STYLE_MIN);
    }

    return returnStr;
}

void LeptonLexer::styleText(int start, int end) {
/* -called whenever text must be (re-) highilighted */
    //qDebug() << "Got called: start=" << start << " end=" << end;
    /*startStyling(0, 0);
    setStyling(end, 0);*/
    //applyStyleTo(0, end, 0);
    QString* text = new QString( this->editor()->text() );
    QTextStream doc(text);
    for (int i = 0; i < 4; i++) {
        //QString c;
        //doc >> c;
        //doc.
        //qDebug() << doc.readLine();
    }
    applyStyleTo(0, end, 0);
}

void LeptonLexer::applyStyleTo(int start, int end, int style) {
/* -applies 'style' between positions 'start' and 'end' inclusively */
    startStyling(start, 0);
    setStyling(end, style);
}

bool LeptonLexer::getLanguageData(const QString& languageFilePath) {
/*
-gets language rules from file
-returns true if the data was successfully extracted, false otherwise
*/
    resetRules();   //reset everything before changing things

    //get the xml file
    QFile xmlFile(languageFilePath);
    QDomDocument langDoc("lang_doc");
    if ( ! langDoc.setContent(&xmlFile) ) return false;

    //get the document element
    QDomElement langElement = langDoc.documentElement();
    if ( langElement.nodeName() != "language") return false;

    //if the current language uses some of the rules of another language, load the other language first
    if ( langElement.hasAttribute("use") ) {
        QString path = QFileInfo(xmlFile).absolutePath().append("/");
        QString file = langElement.attribute("use");
        qDebug() << getLanguageData( path.append(file) );
    }

    //get the language name
    if ( ! langElement.hasAttribute("name") ) return false;
    languageName = langElement.attribute("name").toUtf8().data();

    //get the styling file
    if ( ! langElement.hasAttribute("style") ) return false;
    QString styleFile = langElement.attribute("style");
    QDir pathDir(languageFilePath);
    pathDir.cd("../../styles");
    QString styleFilePath = pathDir.absoluteFilePath(styleFile);
    getStyleFormat(styleFilePath);
    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%% The code above gets executed multiple times because of the 'use' attribute %%
    %%% which causes an other language file to be loaded.  This needs to be fixed. %%
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

    //get number data
    if ( ! langElement.lastChildElement("numbers").isNull() ) {         //if a number expression is specified, extract it
        expressionRules.at(NUMBER_INDEX)->exp.setPattern("");
        QRegularExpression regexp( langElement.lastChildElement("numbers").text() );
        if ( ! regexp.isValid() ) return false;
        expressionRules.at(NUMBER_INDEX)->exp = regexp;
    }
    else {                                                              //if no expression is specified, use default
        expressionRules.at(NUMBER_INDEX)->exp.setPattern("\\b\\d+\\b");
    }

    //get quote/string data
    if ( ! langElement.lastChildElement("quotations").isNull() ) {          //if a quotation expression is specified, extract it
        QDomElement elem = langElement.lastChildElement("quotations");
        blockRules.at(QUOTE_INDEX)->start.setPattern("");
        blockRules.at(QUOTE_INDEX)->end.setPattern("");
        QRegularExpression start( elem.lastChildElement("start").text() );
        QRegularExpression end( elem.lastChildElement("end").text() );
        if ( (! start.isValid()) || (! end.isValid()) ) return false;
        blockRules.at(QUOTE_INDEX)->start = start;
        blockRules.at(QUOTE_INDEX)->end = end;
        bool err = getEscapeFromTo( elem, blockRules.at(QUOTE_INDEX)->escapes );
        if (err == false) return false;
    }
    else {                                                                  //if no expression is specified, use default
        blockRules.at(QUOTE_INDEX)->start.setPattern("\"");
        blockRules.at(QUOTE_INDEX)->end.setPattern("[^\\\\][\"\\r\\n\\f]");
        blockRules.at(QUOTE_INDEX)->escapes.clear();
        blockRules.at(QUOTE_INDEX)->escapes.append( QRegularExpression("\\\\.") );
    }

    //get keyword data
    QDomNodeList nodeList = langElement.elementsByTagName("keywords");  //get lists of keywords
    for ( int i = 0, len = nodeList.count(); i < len; i++) {            //for each keyword list
        //convert node to element
        if ( ! nodeList.at(i).isElement() ) return false;
        QDomElement list = nodeList.at(i).toElement();
        if ( ! list.hasAttribute("type") ) return false;

        //get type of keywords
        bool* err = new bool;
        quint8 type = list.attribute("type").toUInt(err);
        if (err == false || type > KEYWORD_TYPES) return false;

        //create keywords regexp
        QString keywords = list.text().simplified().replace(' ', '|');
        expressionRules[KEYWORD_TYPE_INDEX + type]->exp.setPattern(keywords);
        if ( ! expressionRules[KEYWORD_TYPE_INDEX + type]->exp.isValid() ) {
            //if current expression is not valid, replace it with a blank expression to prevent use while highlighting
            expressionRules[KEYWORD_TYPE_INDEX + type]->exp.setPattern("");
            return false;
        }
    }

    //get expression data
    nodeList = langElement.elementsByTagName("expression");
    for ( int i = 0, len = nodeList.count(); i < len; i++) {
        bool err = getExpressionRule(nodeList.at(i), EXPRESSION_TYPES, EXPRESSION_TYPE_INDEX);
        if (err == false) return false;
    }

    //get line comment rule
    nodeList = langElement.elementsByTagName("linecomment");
    if ( nodeList.length() > 0 ) {
        bool err = getLineRule(nodeList.at(0), LINECOMMENT_TYPES, LINECOMMENT_INDEX);
        if (err == false) return false;
    }
    else { return false; }

    //get line expression data
    nodeList = langElement.elementsByTagName("lineexpression");
    for ( int i = 0, len = nodeList.count(); i < len; i++) {
        bool err = getLineRule(nodeList.at(i), LINEEXPRESSION_TYPES, LINEEXPRESSION_TYPE_INDEX);
        if (err == false) return false;
    }

    //get block comment rule
    nodeList = langElement.elementsByTagName("blockcomment");
    if ( nodeList.length() > 0 ) {
        bool err = getBlockRule(nodeList.at(0), BLOCKCOMMENT_TYPES, BLOCKCOMMENT_INDEX);
        if (err == false) return false;
    }
    else { return false; }

    //get block expression data
    nodeList = langElement.elementsByTagName("blockexpression");
    for ( int i = 0, len = nodeList.count(); i < len; i++) {
        bool err = getBlockRule(nodeList.at(i), BLOCKEXPRESSION_TYPES, BLOCKEXPRESSION_TYPE_INDEX);
        if (err == false) return false;
    }

    return true;
}

bool LeptonLexer::getStyleFormat(const QString& styleFilePath) {
/*
-gets styling data from file
-returns true if data was successfully extracted, false otherwise
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
    //getStyleData( styleElement.lastChildElement("escapedchar"), ESCAPECHAR_STYLE);

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



//~private methodes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void LeptonLexer::resetRules() {
/* -resets all rules */
    for (int i = 0, len = expressionRules.length(); i < len; i++) {
        delete expressionRules[i];
    }
    for (int i = 0, len = lineRules.length(); i < len; i++) {
        delete lineRules[i];
    }
    for (int i = 0, len = blockRules.length(); i < len; i++) {
        delete blockRules[i];
    }
    expressionRules.clear();
    lineRules.clear();
    blockRules.clear();

    /*#############################################################################################
    ### Because QsciLexerCustom, which this class inherits, only allows for 32 different styles, ##
    ### the number of different types of keywords and expressions is limited.  As a result, I    ##
    ### have defined different indicies for each types of rules.  I have also lmited themnumber  ##
    ### of types allowed as follows:                                                             ##
    ###     1  type/style   of number                                                            ##
    ###     1  type/style   of quote (string)                                                    ##
    ###     10 types/styles of keywrds                                                           ##
    ###     7  types/styles of expressions                                                       ##
    ###     1  type/style   of line comment                                                      ##
    ###     5  types/styles of line expressions                                                  ##
    ###     1  type/style   of block comment                                                     ##
    ###     6  types/styles of block expressions                                                 ##
    #############################################################################################*/

    //resize rules to fit the restricted number of styles
    expressionRules.resize(18); //index: 0 = numbers, 1-10 = keywords, 11-17 = user expressions
    lineRules.resize(6);        //index: 0 = line comments, 1-5 = user line expressions
    blockRules.resize(7);       //index: 0 = quotes, 1 = block comments, 2-6 = user block expressions

    for (int i = 0, len = expressionRules.length(); i < len; i++) {
        expressionRules[i] = new ExpressionRuleType;
    }
    for (int i = 0, len = lineRules.length(); i < len; i++) {
        lineRules[i] = new LineRuleType;
    }
    for (int i = 0, len = blockRules.length(); i < len; i++) {
        blockRules[i] = new BlockRuleType;
    }
}

bool LeptonLexer::getExpressionRule(const QDomNode& node, quint8 numberOfTypes, quint8 typeIndex) {
/*
-extracts a expression rule from 'node'
-returns true if the data was successfully extracted, false otherwise
*/
    //convert node to element
    if ( (! node.isElement()) || node.isNull() ) return false;
    QDomElement expElement = node.toElement();
    if ( ! expElement.hasAttribute("type") ) return false;

    //get type of expression
    bool* err = new bool;
    quint8 type = expElement.attribute("type").toUInt(err);
    if (err == false || type >= numberOfTypes) return false;

    //create regexp
    QString expression = expElement.text();
    expressionRules[typeIndex + type]->exp.setPattern(expression);
    if ( ! expressionRules[typeIndex + type]->exp.isValid() ) {
        expressionRules[typeIndex + type]->exp.setPattern("");
        return false;
    }

    return true;
}

bool LeptonLexer::getLineRule(const QDomNode& node, quint8 numberOfTypes, quint8 typeIndex) {
/*
-extracts a line expression rule from 'node'
-returns true if the data was successfully extracted, false otherwise
*/
    //convert node to element
    if ( (! node.isElement()) || node.isNull() ) return false;
    QDomElement expElement = node.toElement();
    if ( ! expElement.hasAttribute("type") ) return false;

    //get type of expression
    bool* err = new bool;
    quint8 type = expElement.attribute("type").toUInt(err);
    if (*err == false || type >= numberOfTypes) return false;

    //create regexp
    QString expression = expElement.text();
    lineRules[typeIndex + type]->exp.setPattern(expression);
    if ( ! lineRules[typeIndex + type]->exp.isValid() ) {
        lineRules[typeIndex + type]->exp.setPattern("");
        return false;
    }

    //get escape sequences
    *err = getEscapeFromTo(expElement, lineRules[typeIndex + type]->escapes);
    return *err;
}

bool LeptonLexer::getBlockRule(const QDomNode& node, quint8 numberOfTypes, quint8 typeIndex) {
/*
-extracts a block expression rule from 'node'
-returns true if the data was successfully extracted, false otherwise
*/
    //convert node to element
    if ( (! node.isElement()) || node.isNull() ) return false;
    QDomElement expElement = node.toElement();
    if ( ! expElement.hasAttribute("type") ) return false;

    //get type of expression
    bool* err = new bool;
    quint8 type = expElement.attribute("type").toUInt(err);
    if (*err == false || type >= numberOfTypes) return false;

    //create regexps
    QString expStart = expElement.lastChildElement("start").text();
    QString expEnd = expElement.lastChildElement("end").text();
    blockRules[typeIndex + type]->start.setPattern(expStart);
    blockRules[typeIndex + type]->end.setPattern(expEnd);
    if ( (! blockRules[typeIndex + type]->start.isValid()) && (! blockRules[typeIndex + type]->end.isValid()) ) {
        blockRules[typeIndex + type]->start.setPattern("");
        blockRules[typeIndex + type]->end.setPattern("");
        return false;
    }

    //get escape sequences
    *err = getEscapeFromTo(expElement, blockRules[typeIndex + type]->escapes);
    return *err;
}

bool LeptonLexer::getEscapeFromTo(QDomElement& element, QVector< QRegularExpression >& escapes) {
/* -gets escape expressions from 'element' and stores them in 'escapes' */
    QDomNodeList nodeList = element.elementsByTagName("escape");
    for ( int i = 0, len = nodeList.length(); i < len; i++) {
        if ( ! nodeList.at(i).isElement() ) return false;
        QRegularExpression exp( nodeList.at(i).toElement().text() );
        if ( ! exp.isValid() ) {
            exp.setPattern("");
            return false;
        }
        escapes.append(exp);
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
    //qDebug() << returnColor.name();

    if ( returnColor.isValid() ) return returnColor;
    else return QColor(0, 0, 0);
}
