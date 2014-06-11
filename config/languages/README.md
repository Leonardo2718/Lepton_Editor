Language Files
==============

Language data is defined in XML files.  These files specify the language keywords
which are to be highlighted as well as other language specific elements such as line
comments and block comments.  The following tags are used to define language data: 

General Tags:
-------------

### &lt;language&gt;
The root element of the file must be ```language```.
The root element must have the attribute ```name``` which specifies the label to be used in the language menu.
Other optional attributes are:
- ```use```: specifies another language file to include as part of the languages (eg. C is part of C++)
- ```style```: specifies the styling/highlighting file to be used for the language (default is used if not specified)

### &lt;keywords&gt;
The ```keywords``` tag is used to create a list of keywords.  Keywords are separated by any
number white spaces (including newlines).  All keywords will by highlighted using the same style.
The ```type``` attribute serves as an ID for the keywords.  Multiple lists of keywords can be created with
different values for ```type``` so that each group of keywords will be highlighted differently.  Only  8 types
(0 to 7) of keywords are currently allowed.

### &lt;blockcomment&gt;
This tag is used to define the start and end expressions (characters) of a block comment.  The child elements
```start``` and ```end``` contain regular expressions (regex) which define the the character sequences used for
the start and end of a block comment, respectively.  All characters, including those for the bounding expressions,
will be highlighted with one style.

### &lt;linecomment&gt;
Like the name suggests, this tag defines the regexp used to identify the start of a line comment.
All characters, including those of the starting expression, will be highlighted with one style.

### &lt;expression&gt;
The ```expression``` tag defines a regexp which will highlight any match.  Each match of the regexp will
be highlighted individually and separately.  This tag uses the ```type``` attribute.  Only 7 types (0 to 6)
of expressions are currently allowed.

### &lt;lineexpression&gt;
The ```lineexpression``` tag is a general form of a line comment. Any time the regexp is matched,
all the characters up to the end of the line will be highlighted.  This tag uses the ```type``` attribute.  
Only 6 types (0 to 5) of expressions are currently allowed.

### &lt;blockexpression&gt;
The ```blockexpression``` tag is a general form of a block comment.  All matches of the ```start``` regexp
will be highlighted up to the first match of the ```end``` regexp.  This tag uses the ```type``` attribute.  
Only 6 types (0 to 5) of expressions are currently allowed.

Optional Tags:
-------------
While none of the above tags are strictly required to form a language file, the following tags will be implemented
using default regexps if they are not explicitly specified.  This is done because I believe them to be general enough
that they should work for most languages.

### &lt;quotations&gt;
This tag is used to define the ```start``` and ```end``` regexps of string/quote.  This tag should only be used for
languages which do not support quotes (in which case it can be left empty), or languages which treat quoted strings
in a special way.

### &lt;numbers&gt;
This tag is used to define the regexp for numbers.  It should only be used if regular sequences of numbers are not treated
as numbers by the language.

Notes
-----

You can use the above information to create language files yourself.

If you chose to include a
language file into your own file, using the 'use' attribute, keep in mind that your file will
override the data in the other file.  For example, if you redefine keywords of 'type' 0, then the
keywords of 'type' 0 defined in the included file will be ignored.  This technique is used in the C++
language file which “uses” the C language file.

If you would like Lepton Editor to support a particular language, please consider creating and
contributing the language file yourself.
