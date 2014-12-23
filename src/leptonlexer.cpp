/*
Project: Lepton Editor
File: leptonlexer.cpp
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: December 22, 2014

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


//include the header that goes with this file
#include "leptonlexer.h"

//include other lepton objects
#include "leptonconfig.h"

//include Qt classes
#include <QFile>
#include <QDomDocument>

#include <QDebug>

//~public methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LeptonLexer::LeptonLexer(QObject *parent) : QsciLexerCustom(parent) {
    languageName = NULL;

    loadStyle( LeptonConfig::mainSettings.getStyleFilePath("default.xml") );

    ruleList.clear();
    loadLanguage();
    setAutoIndentStyle(QsciScintilla::AiMaintain);
}

const char* LeptonLexer::language() const {
/*
-returnes language name
-returns 'NULL' if no language is set
*/
    return languageName.data();
}

QString LeptonLexer::description(int style) const {
/* -returnes description of a style */

    //check for extreme cases first
    /*if (style == DEFAULT_STYLE) return QString("Style #%1 is the default style.").arg(style);
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

    return returnStr;*/
}

void LeptonLexer::styleText(int start, int end) {
/* -called whenever text must be (re-) highilighted */
    //lists of all matches of each rule
    /*applyStyleTo(start, end-start, 0); //apply default style to everything
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
            //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*

            lastEndPosition = endExp.capturedEnd() - 1;
        }
        else if (firstPosition == -1 || ruleType == UNDEF_RULE) break;
        else break;
    }*/

}

void LeptonLexer::applyStyleTo(int start, int length, int style) {
/* -applies 'style' between positions 'start' and 'end' inclusively */
    startStyling(start, 0);
    setStyling(length, style);
}

bool LeptonLexer::loadLanguage(const QString& filePath) {
/*
-loads language tokenization rules from file
-returns true if the data was successfully extracted, false otherwise
*/
    if ( filePath.isEmpty() ){  //if no language file path is specified
        ruleList.clear();           //clear all rules and return
        return true;
    }

    //create a file object to load the language tokenization rules (stored in xml)
    QFile languageFile(filePath);
    QDomDocument langDoc("language_document");

    if (! langDoc.setContent(&languageFile) ) return false;

    //get the documents root element
    QDomElement rootElement = langDoc.documentElement();
    if ( rootElement.nodeName() != "language" ) return false;

    /*###########################################################################################
    ### Note: When the following line recursively calls this method and passes an empty string ##
    ### (`use` attribute not present), this will cause the current rules to be cleared, which  ##
    ### is exactly what we want.  However, if language files link to eachother in a loop       ##
    ### (which doesn't make any practical sense), a stack overflow will occure.                ##
    ###########################################################################################*/
    bool r = loadLanguage( LeptonConfig::mainSettings.getLangFilePath( rootElement.attribute("use", QString() ) ) );
    if (r == false) return false;

    //get the name of the language (displayed in menu)
    if ( !rootElement.hasAttribute("name") || rootElement.attribute("name").isEmpty() ) return false;
    languageName = rootElement.attribute("name").toUtf8();

    //get the element that containes all tokenization rule definitions
    if ( rootElement.lastChildElement("tokenization").isNull() ) return false;
    QDomElement tokenizationRules = rootElement.lastChildElement("tokenization");

    //check if any keywords are defined and, if so, extract them
    QDomNodeList ruleElements = tokenizationRules.elementsByTagName("keywords");
    for (int i = 0, count = ruleElements.count(); i < count; i++) {
        QDomElement rule = ruleElements.at(i).toElement();
        int ruleID = rule.attribute("id").toInt();
        if (ruleID < 0 || ruleID > 31 ) continue;
        TokenRule newRule;
        newRule.name = "KEYWORD";
        newRule.id = ruleID;
        QString exp = rule.firstChild().nodeValue().simplified().replace( QRegularExpression("\\s"), "|").prepend("^");
        newRule.rule.setPattern(exp);
        if ( ! newRule.rule.isValid() ) continue;
        ruleList.append(newRule);
    }

    //check if numbers are used and, if so, implement them
    if (! tokenizationRules.lastChildElement("numbers").isNull() ) {
        int ruleID = tokenizationRules.lastChildElement("numbers").attribute("id").toInt();
        if ( ruleID >= 0 && ruleID <= 31) {
            TokenRule newRule;
            newRule.name = "NUMBER";
            newRule.id = ruleID;
            QString exp = "^\\b\\d+\\b";
            newRule.rule.setPattern(exp);
            if ( newRule.rule.isValid() ) ruleList.append(newRule);
        }
    }

    //extract all other tokenization rules defined purly using regular expressions
    r = extractRulesFrom(tokenizationRules, ruleList);
    if (r == false) return false;

    return true;
}

bool LeptonLexer::loadStyle(const QString& filePath) {
/*
-loads styling data from a file specified using its file info
-returns true if successful, otherwise false
*/
    setDefaultStyleValues();

    //create a file object to load styling data (stored in xml)
    QFile styleFile(filePath);
    QDomDocument styleDoc("styling_document");

    if (! styleDoc.setContent(&styleFile) ) return false;   //if the file does not open or load properly, return with an error

    QDomElement rootElement = styleDoc.documentElement();   //get the root element of the document
    if ( rootElement.nodeName() != "stylelist") return false;

    QDomNodeList styleElements = rootElement.elementsByTagName("style");    //get all elements that define a style

    //for each style element, extract its data if it's valid
    for (int i = 0, count = styleElements.count(); i < count; i++) {
        QDomElement style = styleElements.at(i).toElement();

        if (! style.hasAttribute("id") ) continue;
        int styleID = style.attribute("id").toInt();
        if (styleID < 0 || styleID > 31) continue;

        QDomElement styleItem;  //variable in which to store a styling item from the current style

        if (! style.lastChildElement("color").isNull() ) {
            styleItem = style.lastChildElement("color");
            setColor(LeptonConfig::mainSettings.getColorFromString(styleItem.attribute("value")), styleID);
        }

        if (! style.lastChildElement("background").isNull() ) {
            styleItem = style.lastChildElement("background");
            setPaper(LeptonConfig::mainSettings.getColorFromString(styleItem.attribute("value")), styleID);
        }

        if (! style.lastChildElement("font").isNull() ) {
            styleItem = style.lastChildElement("font");
            setFont(QFont(styleItem.attribute("name")), styleID);
        }
    }

    return true;
}



//~private methodes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool LeptonLexer::setDefaultStyleValues() {
/* -gets the default style values */
    setDefaultPaper( LeptonConfig::mainSettings.getDefaultPaper() );
    setPaper( LeptonConfig::mainSettings.getDefaultPaper(), 0);
    setDefaultColor( LeptonConfig::mainSettings.getDefaultTextColor() );
    setColor( LeptonConfig::mainSettings.getDefaultTextColor(), 0);
    setDefaultFont( LeptonConfig::mainSettings.getDefaultEditorFont() );
    setFont( LeptonConfig::mainSettings.getDefaultEditorFont(), 0);
    return true;
}

bool LeptonLexer::extractRulesFrom(const QDomElement& tokenizationRules, TokenRuleList& rList) {
/*
-extracts all tokenization rules from `rule` and `spanrule` elements in `tokenizationRules`
 and adds them to rList
*/
    QDomNodeList ruleElements = tokenizationRules.elementsByTagName("rule");
    for (int i = 0, count = ruleElements.count(); i < count; i++) {
        QDomElement rule = ruleElements.at(i).toElement();
        int ruleID = rule.attribute("id").toInt();
        if (ruleID < 0 || ruleID > 31 ) continue;
        TokenRule newRule;
        newRule.name = rule.attribute("name");
        newRule.id = ruleID;
        QString exp = rule.firstChild().nodeValue().prepend("^");
        newRule.rule.setPattern(exp);
        if ( ! newRule.rule.isValid() ) continue;
        rList.append(newRule);
    }

    ruleElements = tokenizationRules.elementsByTagName("spanrule");
    for (int i = 0, count = ruleElements.count(); i < count; i++) {
        QDomElement rule = ruleElements.at(i).toElement();
        int ruleID = rule.attribute("id").toInt();
        if (ruleID < 0 || ruleID > 31 ) continue;
        TokenRule newRule;
        newRule.name = rule.attribute("name");
        newRule.id = ruleID;
        QString exp = rule.lastChildElement("open").firstChild().nodeValue().prepend("^");
        newRule.rule.setPattern(exp);
        exp = rule.lastChildElement("close").firstChild().nodeValue().prepend("^");
        newRule.closeRule.setPattern(exp);
        if ( ! newRule.rule.isValid() || ! newRule.closeRule.isValid() ) continue;
        rList.append(newRule);
        extractRulesFrom(rule, newRule.subRules);
    }

    return true;
}
