/*
Project: Lepton Editor
File: leptonlexer.h
Author: Leonardo Banderali
Created: May 8, 2014
Last Modified: May 9, 2014

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

#include <QString>
#include <QDomDocument>
#include <QColor>
#include <Qsci/qscilexercustom.h>
#include <Qsci/qscistyle.h>

/*class StyleNumberType {
    const qint8 number;
    const qint8 lineComment;
    const qint8 blockCommnet;
    const qint8 quote;
    const qint8 escapeChar;
    const qint8 keywords[10];
    const quint8 experssion[7];
    const quint8 lineExp[5];
    const quint8 blockExp[5];

    //StyleNumberType(): number(0), lineComment(1), blockComment(2), quote(3){}
};*/

class LeptonLexer : public QsciLexerCustom
{
    Q_OBJECT

    public:
        //minimum range for styles
        enum StyleTypeValue { NUMBER_STYLE = 0, LINECOMMENT_STYLE = 1, BLOCKCOMMENT_STYLE = 2, QUOTE_STYLE = 3, ESCAPECHAR_STYLE = 4,
                              KEYWORD_STYLE_MIN = 5, KEYWORD_STYLE_MAX = 14,
                              EXPRESSION_STYLE_MIN = 15, EXPRESSION_STYLE_MAX = 21,
                              LINEEXP_STYLE_MIN = 22, LINEEXP_STYLE_MAX = 26,
                              BLOCKEXP_STYLE_MIN = 27, BLOCKEXP_STYLE_MAX = 31 };

        explicit LeptonLexer(QObject *parent = 0);

        const char* language() const;
        /*
        -returnes language name
        -a 'NULL' returned means that no language is set
        */

        QString description(int style) const;
        /* -returnes description of a style */

        void styleText(int start, int end);
        /* -called whenever text must be (re-) highilighted */

        void aplyStyleTo(int start, int end, int style);
        /* -applies 'style' between positions 'start' and 'end' inclusively */

        bool getStyleFormat(const QString& styleFilePath);
        /*
        -gets style info from file
        -returns 'true' if the style information was successfully extracted, false otherwise
        */

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
        /* -converts color defined in a string to a 'QColor' object and returnes it */

    private:
        char* languageName;   //name of language used for syntax highlighting
};

#endif // LEPTONLEXER_H
