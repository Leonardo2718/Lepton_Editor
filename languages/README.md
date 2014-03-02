Language Files
==============

Language data is defined in simple XML files.  These files specify the language keywords
which are to be highlighted as well other language specific elements such as line
comments and multi-line comments.

You can use the following information to create language files yourself. 

Tag List
--------

###&lt;language&gt;
The root element of the file must be &lt;language&gt;.
The root element must have the attribute 'name' which specifies the label to be used in the language menu.
Other optional attributes are:
- use: specifies an other language file to include as part of the languages
- style: specifies the styling/highlighting file to use for the language

### &lt;keywords&gt;
The &lt;keywords&gt; tag is used to create a list of keywords.  Keywords can be separated by any of
number white spaces (including newlines).  All keywords will by highlighted using the same style.
The 'type' attribute serves as an ID for the keywords.  Multiple lists of keywords can be created with
different values for 'type' so that each group of keywords will be highlighted differently.

### &lt;commentstart&gt; and &lt;commentend&gt;
These tags enclose the sequence of characters that delimit block or multi-line comments.

### &lt;linecomment&gt;
Like the name suggests, this tag is used to identify the sequence of characters that denotes a
line comment.

### &lt;expression&gt;
The &lt;expression&gt; tag is one of Lepton Editor's more unique features.  It is used to specify
a Qt/Perl regular expression which can be matched and highlighted. The 'id' attribute is used to
differentiate between the expressions so each can be highlighted differently.  

### &lt;lineexpression&gt;
The &lt;lineexpression&gt; tag is another one of Lepton Editor's features.  This tag is identical
to &lt;expression&gt; except that the whole line after the expression will be highlighted instead of
just the expression itself.  For example, it is used to highlight pre-processor directives in C such
as \#define and \#include.  The 'id' attribute is again used to differentiate between the expressions
so each can be highlighted using a different style.

Notes
-----

You can use the following information to create language files yourself.  If you chose to include a
language file into your own file, using the 'use' attribute, keep in mind that your file data can
overwrite the data in the other file.  For example, if you redefine keywords of 'type' 0, then the
keywords of 'type' 0 defined in the included file will be ignored.  This technique is used in the C++
language file which “uses” the C language file.

If you would like Lepton Editor to support a particular language, please consider creating and
contributing the language file yourself.
