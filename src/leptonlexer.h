/*
Project: Lepton Editor
File: leptonlexer.h
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: May 12, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the definition of the LeptonLexer class which is used to perform highlighting
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

#ifndef LEPTONLEXER_H
#define LEPTONLEXER_H

//define number of types
/*#define DEFAULT_TYPES 1
#define NUMBER_TYPES 1
#define KEYWORD_TYPES 10
#define EXPRESSION_TYPES 7
#define LINECOMMENT_TYPES 1
#define LINEEXPRESSION_TYPES 5
#define QUOTE_TYPES 1
#define BLOCKCOMMENT_TYPES 1
#define BLOCKEXPRESSION_TYPES 6
#define TOTAL_TYPES 32*/

#define DEFAULT_TYPES 1
#define NUMBER_TYPES 1
#define KEYWORD_TYPES 8
#define EXPRESSION_TYPES 7
#define LINECOMMENT_TYPES 1
#define LINEEXPRESSION_TYPES 6
#define QUOTE_TYPES 1
#define BLOCKCOMMENT_TYPES 1
#define BLOCKEXPRESSION_TYPES 6
#define TOTAL_TYPES 32

//define indicies to locate token types in rule arrays
/*#define NUMBER_INDEX 0
#define KEYWORD_TYPE_INDEX 1
#define EXPRESSION_TYPE_INDEX 11
#define LINECOMMENT_INDEX 0
#define LINEEXPRESSION_TYPE_INDEX 1
#define QUOTE_INDEX 0
#define BLOCKCOMMENT_INDEX 1
#define BLOCKEXPRESSION_TYPE_INDEX 2*/

#define NUMBER_INDEX 0
#define KEYWORD_TYPE_INDEX 1
#define EXPRESSION_TYPE_INDEX 9
#define LINECOMMENT_INDEX 0
#define LINEEXPRESSION_TYPE_INDEX 1
#define QUOTE_INDEX 0
#define BLOCKCOMMENT_INDEX 1
#define BLOCKEXPRESSION_TYPE_INDEX 2

//define other useful information
//#define NO_TYPE false
//#define HAS_TYPE true

#include <QString>
#include <QDomDocument>
#include <QColor>
#include <QRegularExpression>
#include <QVector>
#include <Qsci/qscilexercustom.h>
#include <Qsci/qscistyle.h>



class LeptonLexer : public QsciLexerCustom
{
    Q_OBJECT

    public:
        //minimum range for styles
        /*enum StyleTypeValue { NUMBER_STYLE = 0, QUOTE_STYLE = 1, LINECOMMENT_STYLE = 2, BLOCKCOMMENT_STYLE = 3,
                              KEYWORD_STYLE_MIN = 4, KEYWORD_STYLE_MAX = 13,
                              EXPRESSION_STYLE_MIN = 14, EXPRESSION_STYLE_MAX = 20,
                              LINEEXP_STYLE_MIN = 21, LINEEXP_STYLE_MAX = 25,
                              BLOCKEXP_STYLE_MIN = 26, BLOCKEXP_STYLE_MAX = 31 };*/
        enum StyleTypeValue { DEFAULT_STYLE = 0,
                              NUMBER_STYLE = 1,
                              KEYWORD_STYLE_MIN = 2, KEYWORD_STYLE_MAX = 9,
                              EXPRESSION_STYLE_MIN = 10, EXPRESSION_STYLE_MAX = 16,
                              LINECOMMENT_STYLE = 17,
                              LINEEXP_STYLE_MIN = 18, LINEEXP_STYLE_MAX = 23,
                              QUOTE_STYLE = 24,
                              BLOCKCOMMENT_STYLE = 25,
                              BLOCKEXP_STYLE_MIN = 26, BLOCKEXP_STYLE_MAX = 31
                            };

        enum RuleType { EXPRESSION_RULE, LINE_RULE, BLOCK_RULE, UNDEF_RULE };

        //basic rules to match a token
        struct ExpressionRuleType {             //for numbers, keywords, and user defined expressions
            QRegularExpression exp;
            quint8 type;
        };

        struct LineRuleType {                   //for single line comments and user defined line expressions
            QRegularExpression exp;
            QRegularExpression escapes;
            quint8 type;
        };

        struct BlockRuleType {                  //for quotes, block comments, and user defined block expressions
            QRegularExpression start;               //expression to denote start of block
            QRegularExpression end;                 //expression to denote end of block
            QRegularExpression escapes;             //anything that match this expression, inside the block, will be highlighted differently (eg. escape sequences)
            quint8 type;                            //actual type numbe as specified in language definition
        };

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

        bool getLanguageData(const QString& languageFilePath);
        /*
        -gets language rules from file
        -returns true if the data was successfully extracted, false otherwise
        */

        bool getStyleFormat(const QString& styleFilePath);
        /*
        -gets styling data from file
        -returns true if data was successfully extracted, false otherwise
        */

    private:
        char* languageName;   //name of language used for syntax highlighting

        //rules to match tokens and expressions
        QVector< ExpressionRuleType* > expressionRules;
        QVector< LineRuleType* > lineRules;
        QVector< BlockRuleType* > blockRules;

        void resetRules();
        /* -resets all rules */

        bool getExpressionRule(const QDomNode& node, quint8 numberOfTypes, quint8 typeIndex);
        /*
        -extracts a expression rule from 'node'
        -returns true if the data was successfully extracted, false otherwise
        */

        bool getLineRule(const QDomNode& node, quint8 numberOfTypes, quint8 typeIndex);
        /*
        -extracts a line expression rule from 'node'
        -returns true if the data was successfully extracted, false otherwise
        */

        bool getBlockRule(const QDomNode& node, quint8 numberOfTypes, quint8 typeIndex);
        /*
        -extracts a block expression rule from 'node'
        -returns true if the data was successfully extracted, false otherwise
        */

        bool getEscapeFromTo(QDomElement& element, QRegularExpression& escapes);
        /* -gets escape expression from 'element' and stores them in 'escapes' */

        bool getTypedStyleElementData(const QDomNodeList& styleNodeList, StyleTypeValue min, StyleTypeValue max);
        /*
        -retrieves data from elements with the same name but which are differentiated by the value of the 'type' attribute
         (eg. keywords, expression, lineexpression, blockexpression)
        -parameters:
            styleNodeList: list of all the nodes with the same name
            min: lowest style number for the item (used to bound the value of 'type') (eg. 5 for keywords)
            max: highest style number for the item (used to bound the value of 'type') (eg 14 for keywords)
        */

        void getStyleData(QDomElement styleElement, quint8 style);
        /*
        -extracts style info from 'styleElement'
        -if more than one styling item is defined (eg. multiple 'color' tags used), the last item wins
        */

        QColor getColor(QString colorString);
        /* -converts a color defined in a string to a 'QColor' object, using regexp validation, and returnes it */

        int convertRuleIndexToStyle(RuleType rType, int index);
        /*
        -returns the style corresponding to the givin rule type and index
        -returns '-1' if no correspondence was found
        */
};

#endif // LEPTONLEXER_H
