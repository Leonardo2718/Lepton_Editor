/*
Project: Lepton Editor
File: leptonlexer.h
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: December 22, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the declaration of the `LeptonLexer` class.  It is the lexical
    analyzer used by Lepton, primarily as prettyprinter.  It also generates a list of
    tokens representing the file parsed which could be used to perform additionl tasks
    such as syntax completion.  This file also declares addition classes and other types
    used by the lexer.

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

#ifndef LEPTONLEXER_H
#define LEPTONLEXER_H


//include classes that are part of QScintilla
#include <Qsci/qscilexercustom.h>

//include Qt classes
#include <QString>
#include <QRegularExpression>
#include <QList>
#include <QByteArray>
#include <QDomElement>


//declare types used for token identification rules
class TokenRule;

typedef QList<TokenRule> TokenRuleList;

class TokenRule {
    public:
        QString name;
        int id;
        QRegularExpression rule;
        TokenRuleList subRules;
        QRegularExpression closeRule;

        TokenRule() : subRules( TokenRuleList() ), id(0) {}
        ~TokenRule() {}
};


//lexer class declaration
class LeptonLexer : public QsciLexerCustom {
    Q_OBJECT

    public:
        explicit LeptonLexer(QObject *parent = 0);

        const char* language() const;
        /*
        -returnes language name
        -returns 'NULL' if no language is set
        */

        QString description(int style) const;
        /* -returnes description of a style */

        void styleText(int start, int end);
        /* -called whenever text must be (re-) highilighted */

        void applyStyleTo(int start, int length, int style);
        /* -applies 'style' between positions 'start' and 'end' inclusively */

        bool loadLanguage(const QString& filePath = 0);
        /*
            -loads language tokenization rules from file
            -returns true if the data was successfully extracted, false otherwise
        */

        bool loadStyle(const QString& filePath);
        /*  -loads styling data from a file specified using its file info
            -returns true if successful, otherwise false
        */

    private:
        QByteArray languageName;    //name of language used for syntax highlighting
        TokenRuleList ruleList;     //list of rules used to identify tokens

        bool setDefaultStyleValues();
        /* -gets the default style values */

        bool extractRulesFrom(const QDomElement& tokenizationElement, TokenRuleList& rList);
        /*  -extracts all tokenization rules from `rule` and `spanrule` elements in `tokenizationElement`
             and adds them to rList */
};

#endif // LEPTONLEXER_H
