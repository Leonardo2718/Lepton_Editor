/*
Project: Lepton Editor
File: syntaxhighlighter.h
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: March 1, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the definition of the 'SyntaxHighlighter' object wich is responsible
    for applying syntax highlighting to 'Editor' objects.

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

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

//include necessary files and libraries
#include <QSyntaxHighlighter>
#include <QDomDocument>
#include <QFile>
#include <QVector>
#include <QRegularExpression>
#include <QString>
#include <QTextCharFormat>

struct Rule {
/* A struct to hold rule expressions and highlighting format */
    QRegularExpression pattern; //expression to be matched
    QTextCharFormat format;     //format to apply to the expression
};
struct BlockRule {
/* A struct to hold comment block data */
    QRegularExpression start;
    QRegularExpression end;
    QTextCharFormat format;
};
struct  BlockHighlight  {
/* A struct to hold comment block data */
    QRegularExpression start;
    QRegularExpression end;
    int blockType;
};

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT    //implement QObject

    public:
        explicit SyntaxHighlighter(QTextDocument* _editorDocument);
        ~SyntaxHighlighter();

        void useLanguage(QString languageFile = 0);
        /* -loads syntax highlilghting language data from file */

        void useStyle(QString formatFile = 0);
        /* -parses the file with formating data */

    protected:
        void highlightBlock(const QString& docLine);
        /*
        -remplemented pure virtual method
        -apply highlighting rules to given document line
        -will be called automatically by the text engine whenever it is necessary
        -parameters:
            docLine: line from the document to which highlighting rules will be applied
        */

    private:
        enum blockState{ DEFAULT_STATE, IN_BLOCK_COMMENT };  //enumerated type to specify the state of a QTextBlock

        //LanguageBlock language;             //main language data
        //QVector< LanguageBlock > subBlock;  //sub language data

        class LanguageBlock {
        /*
        This class holds the specific rules to match and highlight the keywords and other components of
        a language.  It also contains methods which initialize the rules.
        */
            public:

                /*##############################################################################
                ## The rules which are marked as '@major' have presidence over all others.    ##
                ## Rules which are not marked will always be applied.  For instance, keywords ##
                ## always have syntax highlighting applied, regardless of whether they are    ##
                ## inside comments or not.  As a result, the highlighting for a '@major' rule ##
                ## will overwrite the highlighting of the others.  That is, the comment       ##
                ## highlighting will overwrite that of the previously highlighted keywords    ##
                ## within that comment.                                                       ##
                ##############################################################################*/

                //Universal rules (apply to all languages)
                Rule numbers;
                Rule quotations;                //@major
                Rule escapedChar;

                //single rules
                Rule lineComment;               //@major
                BlockRule blockComment;         //@major

                //rule lists
                QVector< Rule > keywords;
                QVector< Rule > regexps;
                QVector< Rule > lineExps;       //@major
                QVector< BlockRule > blockExps; //@major

                LanguageBlock() {}

                void setUniversalRules() {
                /* -set the Universal rules */
                    numbers.pattern.setPattern("\\b\\d+\\b");
                    quotations.pattern.setPattern("\"[^\"]*\"");
                    escapedChar.pattern.setPattern("\\\\");
                }
        };

        QVector< LanguageBlock > language;  //list of all languages and sub-languages

        void highlightExp(const Rule& r, const QString docLine, unsigned int offset);
        /* -highlights all the occurrences of the expression 'r' in docLine */

        bool highlightEscape(const QString& docLine, int startIndex, int endIndex, const LanguageBlock& lang);
        /*
        -highlihgts escaped character match between 'startIndex' and 'endIndex'
        -return true if the last character in the range is escaped (i.e. it does not mark the end of a string)
        */

        void getLanguageData(QDomElement& root, LanguageBlock& lang);
        /* -parses a  DOM element and retrieves language data from it */

        void getFormat(QDomElement& root, LanguageBlock& lang);
        /* -extracts color data and stores it in a 'LanguageBlock' */

        void setColor(const QDomNode& colorNode, QTextCharFormat& formatObject);
        /*
        -retrieves the color stored in 'colorNode' and sets it in 'fromatObject'
        */
};

#endif // SYNTAXHIGHLIGHTER_H
