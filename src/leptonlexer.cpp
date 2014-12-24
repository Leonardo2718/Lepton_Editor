/*
Project: Lepton Editor
File: leptonlexer.cpp
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: December 24, 2014

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
#include <QStack>

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

    if ( ruleList.isEmpty() ) return;
    const QString& editorText = editor()->text();
    if ( editorText.isEmpty() ) return;

    QStack<TokenRuleList> ruleListStack;    //a stack to keep track of the current token rule list being checked
    ruleListStack.push(ruleList);           //the first rule list to be used is the main rule list

    QString buffer;         //buffer used to store the string being compared against rule expressions
    int charPosition = 0;   //variable to store the position (in the editor text string) of the last character to be added to the buffer

    /*#######################################################################################################
    ### As a rule, once the token matching process is complete for a given token, there will always be one ##
    ### extra character in the buffer.  This character will aslo be loaded first on the next itteratino of ##
    ### the matching process.                                                                              ##
    #######################################################################################################*/

    //check every rule defined to tokenize the text being edited
    while(1) {
        const int ruleCount = ruleListStack.top().size();
        QVector<int> matchState(ruleCount, 1);  //a list to keep track of which rules have partial or full matches (rules that don't match should not even be checked later)

            /*#################################################################################################
            ### The `matchStateq` vector is used to keep track of whether and when a token rule was matched. ##
            ### Each value in the vector corresponds to the "match state" of the token rule with the same    ##
            ### index in the current rule list.  The following encoding is used to store the state:          ##
            ###                                                                                              ##
            ###     +----------+-----------------------------------------------+                             ##
            ###     | value (n)| matche state                                  |                             ##
            ###     +----------+-----------------------------------------------+                             ##
            ###     |        0 | expression was not matched                    |                             ##
            ###     +----------+-----------------------------------------------+                             ##
            ###     |        1 | expression was partially matched              |                             ##
            ###     +----------+-----------------------------------------------+                             ##
            ###     |     >= 2 | expression was matched (n-2) itterations ago. |                             ##
            ###     +----------+-----------------------------------------------+                             ##
            ###                                                                                              ##
            ### Note that for any values greater than 1 (one), the higher the value, the smaller the token   ##
            ### matched must be.  Therefore, assuming that there is only one value of 2 in `matchState`, it  ##
            ### must correspond to the larrgest expression matched.                                          ##
            #################################################################################################*/

        int matchCount = 0;                 //assume that there are initially no matches
        int partialMatchCount = ruleCount;  //assume that all rules initially provide a partial match
        int lastPath = 0;                   //used to keep track of which path was taken before the previous itteration
        int offset = 0;                     //offset to keep track of how many characters must be added to the buffer

        buffer.clear();                         //clear the buffer before adding new characters to it

        while(1) {

            if ( charPosition < editorText.length() ) buffer.append( editorText.at(charPosition) ); //only append a character if there is one to be appended

            //check every token matching rule and update its match state
            for (int i = 0; i < ruleCount; i++) {
                if(matchState[i] == 1) {
                    const QRegularExpression& regex = ruleListStack.top().at(i).rule;
                    QRegularExpressionMatch match = regex.match(buffer, 0, QRegularExpression::PartialPreferFirstMatch);

                    if ( match.hasMatch() ) {
                        matchState[i] = 2;
                        matchCount++;
                        partialMatchCount--;
                        if (match.capturedLength() == buffer.length()) offset = 1;
                    }
                    else if ( match.hasPartialMatch() ) {
                        matchState[i] = 1;
                    }
                    else {
                        matchState[i] = 0;
                        partialMatchCount--;
                    }
                }
                else if (matchState[i] >= 2) {
                    matchState[i] += 1; //increment the state encoding by one to keep track of how small
                                        //  the current token must be compared to the ones more recently matched
                }
            }

            //update the character position and buffer
            if ( matchCount <= 0 && partialMatchCount > 0 && charPosition + 1 < editorText.size() ){
                charPosition++;
                lastPath = 1;
            }
            else if ( matchCount <= 0 && partialMatchCount > 0 && charPosition + 1 >= editorText.size() ) {
                charPosition++;
                buffer.append(" ");

                //check every token matching rule and update its match state
                for (int i = 0; i < ruleCount; i++) {
                    if(matchState[i] == 1) {
                        const QRegularExpression& regex = ruleListStack.top().at(i).rule;
                        QRegularExpressionMatch match = regex.match(buffer, 0, QRegularExpression::PartialPreferFirstMatch);

                        if ( match.hasMatch() ) {
                            matchState[i] = 2;
                            matchCount++;
                            partialMatchCount--;
                        }
                        else if ( match.hasPartialMatch() ) {
                            matchState[i] = 1;
                        }
                        else {
                            matchState[i] = 0;
                            partialMatchCount--;
                        }
                    }
                    else if (matchState[i] >= 2) {
                        matchState[i] += 1; //increment the state encoding by one to keep track of how small
                                            //  the current token must be compared to the ones more recently matched
                    }
                }
                break;
            }
            else if ( matchCount <= 0 && partialMatchCount <= 0 && charPosition + 1 < editorText.size() ) {
                if (lastPath == 0) {
                    charPosition++;
                    buffer.append( editorText.at(charPosition) );
                }
                break;
            }
            else if ( matchCount <= 0 && partialMatchCount <= 0 && charPosition + 1 >= editorText.size() ) {
                if (lastPath == 0) {
                    charPosition++;
                    buffer.append(" ");
                }
                break;
            }
            else if ( matchCount > 0 && partialMatchCount > 0 && charPosition + 1 < editorText.size() ) {
                charPosition++;
            }
            else if ( matchCount > 0 && partialMatchCount > 0 && charPosition + 1 >= editorText.size() ) {
                charPosition++;
                buffer.append(" ");

                //check every token matching rule and update its match state
                for (int i = 0; i < ruleCount; i++) {
                    if(matchState[i] == 1) {
                        const QRegularExpression& regex = ruleListStack.top().at(i).rule;
                        QRegularExpressionMatch match = regex.match(buffer, 0, QRegularExpression::PartialPreferFirstMatch);

                        if ( match.hasMatch() ) {
                            matchState[i] = 2;
                            matchCount++;
                            partialMatchCount--;
                        }
                        else if ( match.hasPartialMatch() ) {
                            matchState[i] = 1;
                        }
                        else {
                            matchState[i] = 0;
                            partialMatchCount--;
                        }
                    }
                    else if (matchState[i] >= 2) {
                        matchState[i] += 1; //increment the state encoding by one to keep track of how small
                                            //  the current token must be compared to the ones more recently matched
                    }
                }
                break;
            }
            else if ( matchCount > 0 && partialMatchCount <= 0 && charPosition + 1 < editorText.size() ){
                if (lastPath == 0 || offset == 1) {
                    charPosition++;
                    buffer.append( editorText.at(charPosition) );
                }
                break;
            }
            else if ( matchCount > 0 && partialMatchCount <= 0 && charPosition + 1 >= editorText.size() ){
                if (lastPath == 0 || offset == 1) {
                    charPosition++;
                    buffer.append(" ");
                }
                break;
            }
        }

        if (matchCount == 0) {
            //reset the highlighting
            applyStyleTo(charPosition - buffer.length() + 1, buffer.length() - 1, 0);
        }
        else if (matchCount >= 1) {

            //get the index of the matching rule
            int i;
            bool tokenIndexFound = false;
            int mState = 1;

            while(!tokenIndexFound) {
                mState++;
                for (i = 0; i < matchState.count(); i++) {
                    if (matchState.at(i) == mState) {
                        tokenIndexFound = true;
                        break;
                    }
                }
            }

            if ( i < matchState.count() ) {

                //get the rule
                const TokenRule& matchedRule = ruleListStack.top().at(i);

                QRegularExpressionMatch m = matchedRule.rule.match(buffer);

                //highlight the matched expression
                applyStyleTo(charPosition - buffer.length() + 1, buffer.length() + 1 - mState, matchedRule.id);
            }
        }

        if ( charPosition >= editorText.length() ) break;
    }
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
        QString exp = rule.firstChild().nodeValue().simplified().replace( QRegularExpression("\\s"), "|").prepend("^\\b(").append(")\\b");
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
            QString exp = "^(\\b\\d+\\b)";
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
    /*QDomNodeList ruleElements = tokenizationRules.elementsByTagName("rule");
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
    }*/

    QDomNodeList nodes = tokenizationRules.childNodes();
    for (int i = 0, count = nodes.count(); i < count; i++) {
        if ( nodes.at(i).isElement() ) {
            QDomElement ruleElement = nodes.at(i).toElement();
            if (ruleElement.tagName() == "rule") {
                int ruleID = ruleElement.attribute("id").toInt();
                if (ruleID < 0 || ruleID > 31 ) continue;
                TokenRule newRule;
                newRule.name = ruleElement.attribute("name");
                newRule.id = ruleID;
                QString exp = ruleElement.firstChild().nodeValue().prepend("^(").append(")");
                newRule.rule.setPattern(exp);
                if ( ! newRule.rule.isValid() ) continue;
                rList.append(newRule);
            }
            else if (ruleElement.tagName() == "spanrule") {
                int ruleID = ruleElement.attribute("id").toInt();
                if (ruleID < 0 || ruleID > 31 ) continue;
                TokenRule newRule;
                newRule.name = ruleElement.attribute("name");
                newRule.id = ruleID;
                QString exp = ruleElement.lastChildElement("open").firstChild().nodeValue().prepend("^(").append(")");
                newRule.rule.setPattern(exp);
                exp = ruleElement.lastChildElement("close").firstChild().nodeValue().prepend("^(").append(")");
                newRule.closeRule.setPattern(exp);
                if ( ! newRule.rule.isValid() || ! newRule.closeRule.isValid() ) continue;
                extractRulesFrom(ruleElement, newRule.subRules);
                rList.append(newRule);
            }
        }
    }

    return true;
}
