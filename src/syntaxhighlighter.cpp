/*
Project: Lepton Editor
File: syntaxhighlighter.cpp
Author: Leonardo Banderali
Created: January 31, 2014
Last Modified: March 16, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file contains the implementation of the 'SyntaxHighlighter' object used to
    apply syntax highlighting to an 'Editor' object.

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

#include "syntaxhighlighter.h"
#include <QDebug>



//~public method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* _editorDocument) : QSyntaxHighlighter(_editorDocument) {
    useLanguage();
}

SyntaxHighlighter::~SyntaxHighlighter(){
    //delete language;
}

void SyntaxHighlighter::useLanguage(QString languageFile) {
/* -loads syntax highlilghting language data from file */
    /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    //$$$ The code following this block may cause a bug in the futur so here is $$$
    //$$$ the code that shouldn't cause the bug in case something happens.      $$$
    //$$$                                                                       $$$
          language.clear();                   //lear the current data
          language.append( LanguageBlock() ); //

          if (languageFile == 0) return;      //if no file was specified return
    //$$$                                                                       $$$
    //$$$ I do not use this code because, at the momment, one of the features   $$$
    //$$$ (using parent languages) cannot work with this code.  Hopfully I will $$$
    //$$$ be able to clean this up in the future so no bug will occure.         $$$
    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

    if (languageFile == 0) {
        language.clear();                   //clear the current data
        language.append( LanguageBlock() ); //
        return;
    }

    QFile file(languageFile);                       //open the language data file
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) return;//return if file was not opened properly
    QDomDocument languageDocument("lang_file");     //create a DOM object from the data file
    bool check = languageDocument.setContent(&file);//set content of the DOM object from the language data file
    file.close();                                   //close the file
    if (!check) return;                             //if the file could not be parsed, return

    QDomElement languageElement = languageDocument.documentElement();       //extract root element
    if( languageElement.nodeName() != "language" ) return;                  //verify that the root element is correct
    QString formatFile =  languageElement.attribute("style", "default.xml");//extract the name of the file which contains highlighting info
    formatFile.prepend("styles/");                                          //add path to file name
    QString parentLanguage = languageElement.attribute("use");              //extract the file name for the parent language
    parentLanguage.prepend("languages/");                                   //add path to file name
    useLanguage(parentLanguage);                                            //apply the parrent language
    getLanguageData(languageElement, language[0]);                          //extract and apply language data
    useStyle(formatFile);
}

void SyntaxHighlighter::useStyle(QString formatFile) {
/* -parses the file with formating data */

    //create DOM object
    QFile file(formatFile);                                             //open the format data file
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text ) ) return;   //return if file was not opened properly
    QDomDocument formatDoc("format_file");                              //create a DOM object from the data file
    bool check = formatDoc.setContent(&file);                           //set content of the DOM object from the language data file
    file.close();                                                       //close the file
    if (!check) return;                             //if the file could not be parsed, return

    //extract data from DOM object
    QDomElement format = formatDoc.documentElement();   //extract root element
    if( format.nodeName() != "format" ) return;         //verify that the root element is correct
    getFormat(format, language[0]);
}



//~protected method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SyntaxHighlighter::highlightBlock(const QString& docLine){
/*
-remplemented pure virtual method
-apply highlighting rules to given document line
-will be called automatically by the text engine whenever it is necessary
-parameters:
    docLine: line from the document to which highlighting rules will be applied
*/
    const LanguageBlock& lang = language.at(0);     //use standard language (in the future this will be used to select nested languages which have not yet been implemented)
    unsigned int stdOffset = 0;                     //standard index at which to start matching all expressions

    if (previousBlockState() == IN_BLOCK_COMMENT) {                         //if the previous line was in a block comment
        QRegularExpressionMatch comEnd = lang.blockComment.end.match(docLine);  //match a closing expressions
        if (comEnd.hasMatch()) {                                                //if a closing expression is found
            setCurrentBlockState(DEFAULT_STATE);                                    //set the current block state to default
            setFormat(0, comEnd.capturedEnd(0), lang.blockComment.format);          //highlight everything up to the end of the closing expression using the block comment format
            stdOffset = comEnd.capturedEnd(0) + 1;                                  //set the standard index offset so only the text aget the closing will be highlighted
        }
        else {
            setCurrentBlockState(IN_BLOCK_COMMENT);                         //set the current block state to insde a block comment
            setFormat(0, docLine.length(), lang.blockComment.format);       //highlight the whole line using using the comment format
            return;                                                         //return since nothing else needs to be done
        }
    }
    else {
        for (int i = 0, l = lang.blockExps.length(); i < l; i++) {          //for each user defined block expression
            if (previousBlockState() == i+2) {                                  //check if the current line is part of the expression
                QRegularExpressionMatch expEnd = lang.blockExps.at(i).end.match(docLine);  //match a closing expressions
                if (expEnd.hasMatch()) {                                                   //if a closing expression is found
                    setCurrentBlockState(DEFAULT_STATE);                                        //set the current block state to default
                    setFormat(0, expEnd.capturedEnd(0), lang.blockExps.at(i).format);           //highlight everything up to the end of the closing expression using the block comment format
                    stdOffset = expEnd.capturedEnd(0) + 1;                                      //set the standard index offset so only the text aget the closing will be highlighted
                }
                else {
                    setCurrentBlockState(i+2);                         //set the current block state to insde a block comment
                    setFormat(0, docLine.length(), lang.blockExps.at(i).format);    //highlight the whole line using using the comment format
                    return;                                                         //return since nothing else needs to be done
                }
            }
        }
    }

    setCurrentBlockState(DEFAULT_STATE);    //set current block state to default to prevent highlighting due to block expressions

    if ( docLine.isEmpty() ) return;    //do not apply highlighting if line is empty

    //highlight keywords
    foreach (const Rule& kList, lang.keywords) {//for each list of keywords
        highlightExp(kList, docLine, stdOffset);    //highlight the keywords
    }

    highlightExp( lang.numbers, docLine, stdOffset);//highlight numbers

    //highlight user defined expressions
    foreach (const Rule& exp, lang.regexps) {
        highlightExp(exp, docLine, stdOffset);
    }

    //highlight @major expressions

    /*######################################################################################
    ## To highlight @major expressions, the line/string is parsed character by character. ##
    ## When a character is found that matches the start of an expressions, highlighting   ##
    ## is applied.  This is done so the program can handle overlaps, such as quotations   ##
    ## inside comments.                                                                   ##
    ######################################################################################*/

    for (int i = stdOffset, strLen = docLine.length(); i < strLen; i++) {               //for every character/index in the string

        //match user defined expressions
        for (int j = 0, l = lang.blockExps.length(); j < l; j++) {      //for every expression
            const QRegularExpressionMatch expStart = lang.blockExps.at(j).start.match(docLine, i);  //look for the start expression
            if ( !expStart.hasMatch() ) continue;                                                   //if no match is found, don't bother checking other conditions
            const QRegularExpressionMatch expEnd = lang.blockExps.at(j).end.match(docLine, i + expStart.capturedLength() ); //look for the end expression
            if (expStart.capturedStart(0) == i && !(lang.blockExps.at(j).start.pattern().isEmpty()) && !(lang.blockExps.at(j).end.pattern().isEmpty()) ) {
            //if the start expression is at the current index and the start and end expresssions are not empty
                if (expEnd.hasMatch()) {                                                //if the end expression is matched
                    setFormat(i, expEnd.capturedEnd(0) - i, lang.blockExps.at(j).format);   //highlight to the end of the expression
                    i = expEnd.capturedEnd(0) + 1;                                          //update the line position index
                }
                else {                                                                  //otherwise
                    setCurrentBlockState(j+2);                                              //change the block state
                    setFormat(i, strLen - i, lang.blockExps.at(j).format);                  //highlight to the end of the line
                    return;                                                                 //no other expressions need to be checked so return
                }
            }
        }

        const QRegularExpressionMatch quote = lang.quotations.pattern.match(docLine, i);    //match a quotation starting at the character
        const QRegularExpressionMatch lineCom = lang.lineComment.pattern.match(docLine, i);
        const QRegularExpressionMatch comStart = lang.blockComment.start.match(docLine, i);

        if (quote.capturedStart(0) == i && !(lang.quotations.pattern.pattern().isEmpty()) ) {   //if the current character is the start of a quotation,
            int len = quote.capturedLength(0);          //get the length of the of the quotation
            setFormat(i, len, lang.quotations.format);  //apply highlighting
            bool endEscaped = highlightEscape(docLine, quote.capturedStart(0), quote.capturedEnd(0), lang); //highlight any escape characters and check if the closing quotation is escaped
            if (endEscaped) i = quote.capturedEnd(0) -2;   //if the closing quotation is escaped, set the index/character so the next iteration of the loop will match the quotation as a start
            else i = quote.capturedEnd(0) + 0;          //otherwise, set the index/character to after the closing quotation so the next iteration can continue to match expressions
        }
        else if (lineCom.capturedStart(0) == i && !(lang.lineComment.pattern.pattern().isEmpty()) ) {   //if the current character is the start of a line comment
            int len = docLine.length() - i;             //get the length up to the end of the string/line
            setFormat(i, len, lang.lineComment.format); //format the line
            return;                                     //return since nothing else can be highlighted
        }
        else if (comStart.capturedStart(0) == i && !(lang.blockComment.start.pattern().isEmpty()) && !(lang.blockComment.end.pattern().isEmpty()) ) {   //if the current character is the start of a block comment
            const QRegularExpressionMatch comEnd = lang.blockComment.end.match(docLine, i + comStart.capturedLength());   //match the end of the block comment
            if (comEnd.hasMatch()) {                                            //if the end expression is matched
                setFormat(i, comEnd.capturedEnd(0) - i, lang.blockComment.format);  //apply comment highlighting
                i = comEnd.capturedEnd(0) + 1;                                      //set the index to after the closing expression
            }
            else {                                                              //otherwise
                setCurrentBlockState(IN_BLOCK_COMMENT);                             //set the block state to be inside a block comment
                setFormat(i, strLen - i, lang.blockComment.format);                 //highlight the rest of the line using the comment format
                return;
            }
        }
        else {                                                                  //if none of the above was matched
            foreach (const Rule& r, lang.lineExps) {                                //for each user defined line expressions
                const QRegularExpressionMatch exp = r.pattern.match(docLine, i);        //match the expression
                if (exp.capturedStart(0) == i && !(r.pattern.pattern().isEmpty()) ) {   //if the expressions is matched
                    int len = docLine.length() - i;                                         //get the length to the end of the line
                    setFormat(i, len, r.format);                                            //highlight everything up to the end of the line
                    return;                                                                 //return because nothing else can be highlighted
                }
            }
        }
    }
}



//~private method implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SyntaxHighlighter::highlightExp(const Rule& r, const QString docLine, unsigned int offset) {
/* -highlights all the occurrences of the expression 'r' in docLine */
    if ( r.pattern.pattern().isEmpty() ) return;    //if no expression is specified, return
    QRegularExpressionMatch match = r.pattern.match(docLine, offset);//match the first occurrence of the expression
    while ( match.hasMatch() ){                     //while a match is found
        int startIndex = match.capturedStart(0);        //get the index of the start of the expression
        int length = match.capturedLength(0);           //get the lindex of the expression
        setFormat(startIndex, length, r.format);        //apply the format
        match = r.pattern.match(docLine, match.capturedEnd(0) + 1);   //find the next match
    }
}

bool SyntaxHighlighter::highlightEscape(const QString& docLine, int startIndex, int endIndex, const LanguageBlock& lang) {
/*
-highlihgts escaped character match between 'startIndex' and 'endIndex'
-return true if the last character in the range is escaped (i.e. it does not mark the end of a string)
*/
    QRegularExpressionMatch match = lang.escapedChar.pattern.match(docLine, startIndex);    //match the first occurrence of an escaped character
    while (match.hasMatch() && match.capturedEnd(0) < endIndex) {  //while an escaped character is matched within the bounds
        setFormat(match.capturedStart(0), 2, lang.escapedChar.format);              //set the format
        match = lang.escapedChar.pattern.match(docLine, match.capturedStart() + 1); //match the next escaped character
        if (match.capturedEnd(0) == endIndex - 1) return 1;                         //if the character is the last, return true to signal that the last character is escaped
    }
    return 0;
}

void SyntaxHighlighter::getLanguageData(QDomElement& root, LanguageBlock& lang) {
/* -parses a  DOM element and retrieves language data from it */
    lang.setUniversalRules();   //set universal expressions

    QDomNodeList keywordsList = root.elementsByTagName("keywords");  //get a list of all the keyword nodes
    for (int i = 0, len = keywordsList.length(); i < len; i++) {    //for every list of keywords
        int keywordType = keywordsList.at(i).attributes().namedItem("type").namedItem("#text").toText().data().toInt();//retrieve the type of keyword held by the element
        QString keywords = keywordsList.at(i).namedItem("#text").toText().data();       //get the list of keywords
        keywords = keywords.simplified();                                               //remove unnecessary white spaces
        keywords = keywords.replace( QRegularExpression("\\s"), "|");                   //replace all spaces with pipes (used for regexp)
        keywords = keywords.prepend("\\b(");                                            //add components of regexp
        keywords = keywords.append(")\\b");                                             //
        if (keywordType >= lang.keywords.length()) lang.keywords.resize(keywordType+1); //if there is no keywords list to which to assign the new list, resive the list so there is
        lang.keywords[keywordType].pattern.setPattern(keywords);                        //set the regexp in the list of keywords
    }

    QDomNodeList lineC = root.elementsByTagName("linecomment");         //retrieve nodes with line comment expressions
    if ( !lineC.isEmpty() ) {                                           //if a node is found
        QString expression = lineC.at(0).namedItem("#text").toText().data();//get the expression
        expression = expression.simplified();                               //remove unnecessary white spaces
        expression = lang.lineComment.pattern.escape(expression);           //escape characters so that the expression can be identified
        expression.append("[^\\n]*");                                       //add needed components to complete the expression
        lang.lineComment.pattern.setPattern(expression);                    //set the expression
    }

    QDomNodeList commentStart = root.elementsByTagName("commentstart");
    if (commentStart.length() != 0) {
        QString expression = commentStart.at(0).namedItem("#text").toText().data();
        expression = expression.simplified();
        expression = lang.blockComment.start.escape(expression);
        lang.blockComment.start.setPattern(expression);
    }

    QDomNodeList commentEnd = root.elementsByTagName("commentend");
    if (commentEnd.length() != 0) {
        QString expression = commentEnd.at(0).namedItem("#text").toText().data();
        expression = expression.simplified();
        expression = lang.blockComment.end.escape(expression);
        lang.blockComment.end.setPattern(expression);
    }

    QDomNodeList regexps = root.elementsByTagName("expression");                //get a list of the user defined regular expressions
    for (int i = 0, len = regexps.length(); i < len; i++) {                     //for every expression
        int type = regexps.at(i).attributes().namedItem("type").toText().data().toInt();  //get the expression type
        QString exp = regexps.at(i).firstChild().toText().data();                   //get the expression itself
        exp = exp.simplified();
        if (type >= lang.regexps.length()) lang.regexps.resize(type+1);                 //if the space to store the expression does not exist, resize the list to create space
        lang.regexps[type].pattern.setPattern(exp);                                //set the pattern
        if ( !(lang.regexps.at(type).pattern.isValid()) ) lang.regexps.remove(type);    //if the pattern is not valid, delete it
    }

    QDomNodeList lineexps = root.elementsByTagName("lineexpression");
    for (int i = 0, len = lineexps.length(); i < len; i++) {
        int type = lineexps.at(i).attributes().namedItem("type").toText().data().toInt();
        QString exp = lineexps.at(i).firstChild().toText().data();
        exp = exp.simplified();
        if (type >= lang.lineExps.length()) lang.lineExps.resize(type+1);
        lang.lineExps[type].pattern.setPattern(exp);
        if ( !(lang.lineExps.at(type).pattern.isValid()) ) lang.lineExps.remove(type);
    }

    //highlight user defined block expressions
    QDomNodeList blockExps = root.elementsByTagName("blockexpression");
    for (int i = 0, len = blockExps.length(); i < len; i++) {   //for every expression defined
        int type = blockExps.at(i).attributes().namedItem("type").toText().data().toInt();//get the type of the expressions

        if (type >= lang.blockExps.length()) lang.blockExps.resize(type+1); //resize the vector of expressions if needed

        //get the start expression
        QDomNode start = blockExps.at(i).namedItem("start");
        //qDebug() << start.firstChild().toText().data();
        if (commentStart.length() != 0) {
            QString expression = start.namedItem("#text").toText().data();
            expression = expression.simplified();
            lang.blockExps[type].start.setPattern(expression);
        }

        //get the end expression
        QDomNode end = blockExps.at(i).namedItem("end");
        if (commentEnd.length() != 0) {
            QString expression = end.namedItem("#text").toText().data();
            expression = expression.simplified();
            lang.blockExps[type].end.setPattern(expression);
        }

        //if either the start or end expression is invalid, remove the whole block expression
        if ( !(lang.blockExps.at(type).start.isValid()) || !(lang.blockExps.at(type).start.isValid()) )
            lang.blockExps.remove(type);
    }
}

void SyntaxHighlighter::getFormat(QDomElement& root, LanguageBlock& lang) {
/* -extracts color data and stores it in a 'LanguageBlock' */
    QDomNodeList keywords = root.elementsByTagName("keywords"); //get keyword type data
    for (int i = 0, len = keywords.length(); i < len; i++) {    //for every type of keyword
        QDomNode color = keywords.at(i).namedItem("color");     //extract color data
        int type = keywords.at(i).attributes().namedItem("type").namedItem("#text").toText().data().toInt();   //get the type of keyword to which to apply the format
        if (type >= lang.keywords.length()) continue;           //if the type is not defined (index out of bounds) do not implement highlighting rules
        setColor(color, lang.keywords[type].format);            //set the color
    }

    QDomNodeList exp = root.elementsByTagName("expression");
    for (int i = 0, len = exp.length(); i < len; i++) {
        QDomNode color = exp.at(i).namedItem("color");
        int type = exp.at(i).attributes().namedItem("type").toText().data().toInt();
        if (type >= lang.regexps.length()) continue;
        setColor(color, lang.regexps[type].format);
    }

    QDomNodeList linexp = root.elementsByTagName("lineexpression");
    for (int i = 0, len = linexp.length(); i < len; i++) {
        QDomNode color = linexp.at(i).namedItem("color");
        int type = linexp.at(i).attributes().namedItem("type").toText().data().toInt();
        if (type >= lang.lineExps.length()) continue;
        setColor(color, lang.lineExps[type].format);
    }

    QDomNodeList blockexp = root.elementsByTagName("blockexpression");
    for (int i = 0, len = blockexp.length(); i < len; i++) {
        QDomNode color = blockexp.at(i).namedItem("color");
        int type = blockexp.at(i).attributes().namedItem("type").toText().data().toInt();
        if (type >= lang.blockExps.length()) continue;
        setColor(color, lang.blockExps[type].format);
    }

    QDomNode color = root.namedItem("numbers").namedItem("color");
    setColor(color, lang.numbers.format);

    color = root.namedItem("quotations").namedItem("color");
    setColor(color, lang.quotations.format);

    color = root.namedItem("escapedchar").namedItem("color");
    setColor(color, lang.escapedChar.format);

    color = root.namedItem("linecomment").namedItem("color");
    setColor(color, lang.lineComment.format);

    color = root.namedItem("blockcomment").namedItem("color");
    setColor(color, lang.blockComment.format);
}

void SyntaxHighlighter::setColor(const QDomNode& colorNode, QTextCharFormat& formatObject){
/*
-retrieves the color stored in 'colorNode' and sets it in 'fromatObject'
*/
    if (colorNode.isNull()) return;     //return if the no node was passed

    QString colorString = colorNode.namedItem("#text").toText().data(); //retrieve string from the colorNode
    colorString = colorString.trimmed();                                //remove spaces at the start and end of the string

    //match RGB value
    QStringList colorRGB = colorString.split( QRegExp("\\s") );     //split the string
    bool k1, k2, k3;
    if ( colorRGB.size() == 3) {        //if the string can be split into three parts
        int r = colorRGB[0].toInt(&k1);     //convert values to integers
        int g = colorRGB[1].toInt(&k2);     //
        int b = colorRGB[2].toInt(&k3);     //
        if (k1 && k2 && k3) {               //if the conversion was successful
            QColor c( r, g, b);                 //interpret each part as being an RGB value and make the corresponding color
            formatObject.setForeground(c);      //set the interpreted color
            return;
        }
    }

    colorString.remove( QRegExp("\\s") );   //remove all white spaces since expected values should not have any

    /*##################################################################################
    ## Color names do not have any white spaces between them.  However, users can add ##
    ## them in (for readability) when creating formatting files since they will be    ##
    ## explicitly removed.                                                            ##
    ##################################################################################*/

    //match hex value
    if ( colorString.contains( '#' ) && (
         colorString.size() == 4 || colorString.size() == 7 || colorString.size() == 9 ||
         colorString.size() == 10 || colorString.size() == 13 )
       )
    {
        formatObject.setForeground( QColor(colorString) );
    }

    colorString = colorString.toLower();    //make lower case to match color names

    //match color names
    QStringList cNames = QColor::colorNames();                  //get a list of all colors that Qt knows about
    for (int i = 0, l = cNames.length(); i < l; i++) {          //for each color in the list
        if (colorString == cNames[i]) {                             //if the retrieved string matches a known color name
            formatObject.setForeground( QColor(cNames[i]) );            //set the color
            break;                                                      //break
        }
    }
}
