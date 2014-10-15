/*
Project: Lepton Editor
File: leptonlexer.cpp
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: October 14, 2014

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
#include "leptonconfig.h"



//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonLexer::LeptonLexer(QObject *parent) : QsciLexerCustom(parent) {
    languageName = NULL;

    resetRules();
    getLanguageData();
    setAutoIndentStyle(QsciScintilla::AiMaintain);
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
    if (style == DEFAULT_STYLE) return QString("Style #%1 is the default style.").arg(style);
    if (style < 0 || style > TOTAL_TYPES) return QString("Style #%1 is not defined.").arg(style);

    QString returnStr;          //dummy string to store return value
    returnStr = QString("Style #%1 is the style for ").arg(style);

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
    //lists of all matches of each rule
    applyStyleTo(start, end-start, 0); //apply default style to everything
    QVector< QRegularExpressionMatchIterator > expressionRuleMatchList( expressionRules.length() );
    QVector< QRegularExpressionMatchIterator > lineRuleMatchList( lineRules.length() );
    QVector< QRegularExpressionMatchIterator > blockRuleMatchList( blockRules.length() );

    //list of expressions to be ignored (assume none)
    QVector< bool > ignorExpressionRule( expressionRules.length(), false );
    QVector< bool > ignorLineRule( lineRules.length(), false );
    QVector< bool > ignorBlockRule( blockRules.length(), false );

    //go through all expressions and find which ones to ignore; all regex that are empty or invalid should be ignored
    for (int i = 0, len = expressionRules.length(); i < len; i++) {
        if ( (! expressionRules.at(i)->exp.isValid()) || expressionRules.at(i)->exp.pattern().isEmpty() ) ignorExpressionRule[i] = true;
    }
    for (int i = 0, len = lineRules.length(); i < len; i++) {
        if ( (! lineRules.at(i)->exp.isValid()) || lineRules.at(i)->exp.pattern().isEmpty() ) ignorLineRule[i] = true;
    }
    for (int i = 0, len = blockRules.length(); i < len; i++) {
        if ( (! blockRules.at(i)->start.isValid()) || blockRules.at(i)->start.pattern().isEmpty() ||
             (! blockRules.at(i)->end.isValid()) || blockRules.at(i)->end.pattern().isEmpty() ) ignorBlockRule[i] = true;
    }

    //get the text being edited
    QString text( this->editor()->text() );

    //match all expressions
    for (int i = expressionRules.length() - 1; i >= 0; i--) {
        if ( ignorExpressionRule[i] )  continue;
        expressionRuleMatchList[i] = expressionRules.at(i)->exp.globalMatch(text);
    }
    for (int i = lineRules.length() - 1; i >= 0; i--) {
        if ( ignorLineRule[i] ) continue;
         lineRuleMatchList[i] = lineRules.at(i)->exp.globalMatch(text);
    }
    for (int i = blockRules.length() - 1; i >= 0; i--) {
        if ( ignorBlockRule[i] )  continue;
        blockRuleMatchList[i] = blockRules.at(i)->start.globalMatch(text);
    }

    quint32 lastEndPosition = 0;  //position where last expression ended
    while (true) {
        //select the first expression matched by performing a linear search
        quint32 firstPosition = -1;         //start position of first match
        RuleType ruleType = UNDEF_RULE;     //rule type of match
        quint8 ruleIndex = 0;               //index of the rule in rule type array
        for (int i = expressionRuleMatchList.length() - 1; i >= 0; i--) {
            //if the expression must be ignored (eg. if it's inside a block expressions), move to the next matched expression
            if (expressionRuleMatchList[i].hasNext() && lastEndPosition != 0 && expressionRuleMatchList[i].peekNext().capturedStart() <= lastEndPosition) {
                expressionRuleMatchList[i].next();
                i++;
                continue;
            }
            //if an expression matched is positioned before the previously checked match, consider it to be first and check the next match
            if (expressionRuleMatchList[i].hasNext() && (quint32)(expressionRuleMatchList[i].peekNext().capturedStart()) < firstPosition) {
                firstPosition = expressionRuleMatchList[i].peekNext().capturedStart();
                ruleType = EXPRESSION_RULE;
                ruleIndex = i;
            }
        }
        for (int i = lineRuleMatchList.length() - 1; i >= 0; i--) {
            if (lineRuleMatchList[i].hasNext() && lastEndPosition != 0 && lineRuleMatchList[i].peekNext().capturedStart() <= lastEndPosition) {
                lineRuleMatchList[i].next();
                i++;
                continue;
            }
            if (lineRuleMatchList[i].hasNext() && (quint32)(lineRuleMatchList[i].peekNext().capturedStart()) < firstPosition) {
                firstPosition = lineRuleMatchList[i].peekNext().capturedStart();
                ruleType = LINE_RULE;
                ruleIndex = i;
            }
        }
        for (int i = blockRuleMatchList.length() - 1; i >= 0; i--) {
            if (blockRuleMatchList[i].hasNext() && lastEndPosition != 0 && blockRuleMatchList[i].peekNext().capturedStart() <= lastEndPosition) {
                blockRuleMatchList[i].next();
                i++;
                continue;
            }
            if (blockRuleMatchList[i].hasNext() && (quint32)(blockRuleMatchList[i].peekNext().capturedStart()) < firstPosition) {
                firstPosition = blockRuleMatchList[i].peekNext().capturedStart();
                ruleType = BLOCK_RULE;
                ruleIndex = i;
            }
        }

        //highlight the expression/rule match selected
        if (ruleType == EXPRESSION_RULE) {
            //if the rule is for an expression, get the corresponding style and highlight the text
            if ( !(expressionRuleMatchList[ruleIndex].hasNext()) || firstPosition != expressionRuleMatchList[ruleIndex].peekNext().capturedStart()) break;//sanity check to make sure the correct expression/rule is selected
            int style = convertRuleIndexToStyle(EXPRESSION_RULE, ruleIndex);
            if (style < 0) break;
            applyStyleTo(firstPosition, expressionRuleMatchList[ruleIndex].peekNext().capturedLength(), style);
            lastEndPosition = expressionRuleMatchList[ruleIndex].next().capturedEnd() - 1;  //redefine the position at which to start checking for matches
        }
        else if (ruleType == LINE_RULE) {
            //if the rule is for a line, get the corresponding style, find the end of the line, highlight everything up to the end of the line (%checks for escapes not implemented yet%)
            if ( !(lineRuleMatchList[ruleIndex].hasNext()) || firstPosition != lineRuleMatchList[ruleIndex].peekNext().capturedStart()) break;
            int style = convertRuleIndexToStyle(LINE_RULE, ruleIndex);
            if (style < 0) break;
            lineRuleMatchList[ruleIndex].next();
            QRegularExpressionMatch endExp = QRegularExpression( QString("([^\\\\][\r\n]{1,2})|($)") ).match(text, firstPosition);
            int endOfLine = endExp.capturedEnd();
            if (endOfLine < 0) continue;
            applyStyleTo(firstPosition, endOfLine - firstPosition, style);
            lastEndPosition = endOfLine - 1;
        }
        else if (ruleType == BLOCK_RULE) {
            //if the rule is for a block, get the corresponding style, find the end of the block, highlight everything withing the block (%checks for escapes not implemented yet%)
            if ( !(blockRuleMatchList[ruleIndex].hasNext()) || firstPosition != blockRuleMatchList[ruleIndex].peekNext().capturedStart()) break;
            int style = convertRuleIndexToStyle(BLOCK_RULE, ruleIndex);
            if (style < 0) break;
            QRegularExpressionMatch endExp = blockRules[ruleIndex]->end.match(text, firstPosition + 1);
            if (endExp.capturedStart() < 0) continue;
            applyStyleTo(firstPosition, endExp.capturedEnd() - firstPosition, style);

            //highlight escapes
            /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
            //$ This code does not currently work so it's commented out. At the moment, the color change does not work because $$$
            //$ a new style is probably required.  I cannot simply temporarily change the color of a style.                    $$$
            //$                                                                                                                $$$
                if ( (! blockRules[ruleIndex]->escapes.pattern().isEmpty()) &&  blockRules[ruleIndex]->escapes.isValid() ) {
                    QRegularExpressionMatchIterator escapeMatches =  blockRules[ruleIndex]->escapes.globalMatch(text, firstPosition+startExp.capturedLength() -1);
                    while ( escapeMatches.hasNext() ) {
                        if ( escapeMatches.peekNext().capturedStart() > endExp.capturedStart() ) break; //breack if escape is outside of block
                        QColor nonEscapeColor = color(style);
                        QColor escapeColor(255, 255, 255);
                        setColor( escapeColor, style );
                        //colorChanged(escapeColor, style);
                        applyStyleTo( escapeMatches.peekNext().capturedStart(), escapeMatches.peekNext().capturedLength(), style);
                        escapeMatches.next();
                        setColor(nonEscapeColor, style);
                    }
                }
            //$                                                                                                                $$$
            //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

            lastEndPosition = endExp.capturedEnd() - 1;
        }
        else if (firstPosition == -1 || ruleType == UNDEF_RULE) break;
        else break;
    }
}

void LeptonLexer::applyStyleTo(int start, int length, int style) {
/* -applies 'style' between positions 'start' and 'end' inclusively */
    startStyling(start, 0);
    setStyling(length, style);
}

bool LeptonLexer::getLanguageData(const QString& languageFilePath) {
/*
-gets language rules from file
-returns true if the data was successfully extracted, false otherwise
*/
    //reset everything before changing things
    resetRules();

    //if no language file is specified, assume no syntax highlighting
    if ( languageFilePath.isEmpty() ) {
        getDefaultStyle();
        return true;
    }

    //get the xml file
    QFile xmlFile(languageFilePath);
    QDomDocument langDoc("lang_doc");
    if ( ! langDoc.setContent(&xmlFile) ) return false;

    //get the document element
    QDomElement langElement = langDoc.documentElement();
    if ( langElement.nodeName() != "language") return false;

    //if the current language uses some of the rules of another language, load the other language first
    if ( langElement.hasAttribute("use") ) {
        QString file = langElement.attribute("use");
        getLanguageData( LeptonConfig::mainSettings.getLangFilePath(file) );
    }

    //get the language name
    if ( ! langElement.hasAttribute("name") ) return false;
    languageName = langElement.attribute("name").toUtf8().data();

    //get the styling file
    if ( langElement.hasAttribute("style") ) {
        QString styleFile = langElement.attribute("style");
        QString styleFilePath = LeptonConfig::mainSettings.getStyleFilePath(styleFile);
        if ( styleFilePath.isEmpty() ) styleFilePath = LeptonConfig::mainSettings.getStyleFilePath("default.xml");
        bool err  = getStyleFormat( styleFilePath );
        if (err == false) return false;
    }
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
    if ( ! langElement.lastChildElement("quotations").isNull() ) {      //if a quotation expression is specified, extract it
        QDomElement elem = langElement.lastChildElement("quotations");
        blockRules.at(QUOTE_INDEX)->start.setPattern("");
        blockRules.at(QUOTE_INDEX)->end.setPattern("");
        QRegularExpression start( elem.lastChildElement("start").text() );
        QRegularExpression end( elem.lastChildElement("end").text() );
        if ( (! start.isValid()) || (! end.isValid()) || elem.hasAttribute("type") ) return false;
        blockRules.at(QUOTE_INDEX)->start = start;
        blockRules.at(QUOTE_INDEX)->end = end;
        bool err = getEscapeFromTo( elem, blockRules.at(QUOTE_INDEX)->escapes );
        if (err == false) return false;
    }
    else {                                                              //if no expression is specified, use default
        blockRules.at(QUOTE_INDEX)->start.setPattern("\"");
        blockRules.at(QUOTE_INDEX)->end.setPattern("([^\\\\][\"\\r\\n\\f])|$");
        blockRules.at(QUOTE_INDEX)->escapes.setPattern("\\\\(.?)");
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
        keywords = keywords.prepend("\\b(");
        keywords = keywords.append(")\\b");
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
    //else { return false; }

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
    //else { return false; }

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
    getDefaultStyle();

    //get the xml file
    QFile xmlFile(styleFilePath);
    QDomDocument styleDoc("style_doc");
    if ( ! styleDoc.setContent(&xmlFile) ) return false;

    //get the document element
    QDomElement styleElement = styleDoc.documentElement();
    if ( styleElement.nodeName() != "format") return false;

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
    ###     1  default type                                                                      ##
    ###     1  type/style   of number                                                            ##
    ###     1  type/style   of quote (string)                                                    ##
    ###     8  types/styles of keywrds                                                           ##
    ###     7  types/styles of expressions                                                       ##
    ###     1  type/style   of line comment                                                      ##
    ###     6  types/styles of line expressions                                                  ##
    ###     1  type/style   of block comment                                                     ##
    ###     6  types/styles of block expressions                                                 ##
    #############################################################################################*/

    //resize rules to fit the restricted number of styles
    expressionRules.resize(NUMBER_TYPES + KEYWORD_TYPES + EXPRESSION_TYPES);    //index: 0 = numbers, 1-8 = keywords, 9-15 = user expressions
    lineRules.resize(LINECOMMENT_TYPES + LINEEXPRESSION_TYPES);                 //index: 0 = line comments, 1-6 = user line expressions
    blockRules.resize(QUOTE_TYPES + BLOCKCOMMENT_TYPES + BLOCKEXPRESSION_TYPES);//index: 0 = quotes, 1 = block comments, 2-7 = user block expressions

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

    //if a type is required, extract it from the 'type' attribute
    quint8 type = 0;
    if (numberOfTypes > 1) {
        if ( ! expElement.hasAttribute("type") ) return false;
        bool* err = new bool;
        type = expElement.attribute("type").toUInt(err);
        if (err == false || type >= numberOfTypes) return false;
    }

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

    //if a type is required, extract it from the 'type' attribute
    quint8 type = 0;
    bool* err = new bool;
    if (numberOfTypes > 1) {
        if ( ! expElement.hasAttribute("type") ) return false;
        type = expElement.attribute("type").toUInt(err);
        if (*err == false || type >= numberOfTypes) return false;
    }

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

    //if a type is required, extract it from the 'type' attribute
    quint8 type = 0;
    bool* err = new bool;
    if (numberOfTypes > 1) {
        if ( ! expElement.hasAttribute("type") ) return false;
        type = expElement.attribute("type").toUInt(err);
        if (*err == false || type >= numberOfTypes) return false;
    }

    //create regexps
    QString expStart = expElement.lastChildElement("start").text();
    QString expEnd = expElement.lastChildElement("end").text();
    expEnd.prepend("(");
    expEnd.append(")|($)");
    blockRules[typeIndex + type]->start.setPattern(expStart);
    blockRules[typeIndex + type]->end.setPattern(expEnd);
    if ( (! blockRules[typeIndex + type]->start.isValid()) || (! blockRules[typeIndex + type]->end.isValid()) ) {
        blockRules[typeIndex + type]->start.setPattern("");
        blockRules[typeIndex + type]->end.setPattern("");
        return false;
    }

    //get escape sequences
    *err = getEscapeFromTo(expElement, blockRules[typeIndex + type]->escapes);
    return *err;
}

bool LeptonLexer::getEscapeFromTo(QDomElement& element, QRegularExpression &escapes) {
/* -gets escape expression from 'element' and stores them in 'escapes' */
    QDomElement escapeElement = element.lastChildElement("escapes");
    if ( escapeElement.isNull() ) {
        escapes.setPattern("");
        return true;
    }
    QString exp = escapeElement.text();
    if ( exp.isEmpty() ) return false;
    escapes.setPattern(exp);
    if ( ! escapes.isValid() ) {
        escapes.setPattern("");
        return false;
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

        QDomElement styleElement = styleNodeList.at(i).toElement(); //convert node to element

        if ( styleElement.attribute("type").isEmpty() ) continue;   //extract the 'type' id number
        quint8 typeNumber = styleElement.attribute("type").toInt();

        if ( min + typeNumber > max) continue;                      //if 'typeNumber' is outside the range of allocated style numbers, continue to next node
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

bool LeptonLexer::getDefaultStyle() {
/* -gets the default style values */
    setDefaultPaper( LeptonConfig::mainSettings.getDefaultPaper() );
    setDefaultColor( LeptonConfig::mainSettings.getDefaultTextColor() );
    setDefaultFont( LeptonConfig::mainSettings.getDefaultEditorFont() );

    //set styling in editor settings
    //if (editor() == NULL) return false;
    //editor()->setUnmatchedBraceBackgroundColor( GeneralConfig::getDefaultPaper() );
    //editor()->setMatchedBraceBackgroundColor( GeneralConfig::getDefaultPaper() );
    //editor()->setWhitespaceVisibility( GeneralConfig::getWhiteSpaceVisibility() );
    //editor()->setWhitespaceForegroundColor( GeneralConfig::getWhiteSpaceColor() );

    return true;
}

QColor LeptonLexer::getColor(QString colorString) {
/* -converts a color defined in a string to a 'QColor' object, using regexp validation, and returnes it */
    if ( colorString.isEmpty() ) return QColor();   //return if no color is specified

    //cleen up the string
    colorString = colorString.simplified();
    colorString = colorString.trimmed();

    QColor returnColor(0, 0, 0);                    //dummy color to be returned
    int p = 0;                                      //used for regexp validator

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

    if ( returnColor.isValid() ) return returnColor;
    else return QColor(0, 0, 0);
}

int LeptonLexer::convertRuleIndexToStyle(RuleType rType, int index) {
/*
-returns the style corresponding to the givin rule type and index
-returns '-1' if no correspondence was found
*/
    if (rType == EXPRESSION_RULE) {
        if (index == NUMBER_INDEX) return NUMBER_STYLE;
        else if ((index >= KEYWORD_TYPE_INDEX) && (index <= KEYWORD_TYPE_INDEX + KEYWORD_TYPES)) return KEYWORD_STYLE_MIN + index - KEYWORD_TYPE_INDEX;
        else if ((index >= EXPRESSION_TYPE_INDEX) && (index <= EXPRESSION_TYPE_INDEX + EXPRESSION_TYPES)) return EXPRESSION_STYLE_MIN + index - EXPRESSION_TYPE_INDEX;
    }
    else if (rType == LINE_RULE) {
        if (index == LINECOMMENT_INDEX) return LINECOMMENT_STYLE;
        else if ((index >= LINEEXPRESSION_TYPE_INDEX) && (index <= LINEEXPRESSION_TYPE_INDEX + LINEEXPRESSION_TYPES)) return LINEEXP_STYLE_MIN + index - LINEEXPRESSION_TYPE_INDEX;
    }
    else if (rType == BLOCK_RULE) {
        if (index == QUOTE_INDEX) return QUOTE_STYLE;
        else if (index == BLOCKCOMMENT_INDEX) return BLOCKCOMMENT_STYLE;
        else if ((index >= BLOCKEXPRESSION_TYPE_INDEX) && (index <= BLOCKEXPRESSION_TYPE_INDEX + BLOCKEXPRESSION_TYPES)) return BLOCKEXP_STYLE_MIN + index - BLOCKEXPRESSION_TYPE_INDEX;
    }
    return 0;
}
