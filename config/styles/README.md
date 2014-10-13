Style Definition
================

Styles used for syntax highlighting are defined in xml files.  Lepton currently only allows changing the color of the text.  This may change in the future.  Bellow is a list of the tags used to define syntax highlighting styles.

###&lt;format&gt;
This is the root element of the xml file and has not attributes

###&lt;keywords&gt;
This element contains the style definition for language keywords.  The ```type``` attribute can be used to specify the list
of keywords (as defined in the language xml file) to which the style will be applied.  
######Child element(s):
- &lt;color&gt;

###&lt;numbers&gt;
This element contains the style definition for numbers.
######Child element(s):
- &lt;color&gt;

###&lt;linecomment&gt;
This element contains the style definition for line comments.
######Child element(s):
- &lt;color&gt;

###&lt;blockcomment&gt;
This element contains the style definition for block comments.
######Child element(s):
- &lt;color&gt;

###&lt;quotations&gt;
This element contains the style definition for quotations (strings).
######Child element(s):
- &lt;color&gt;

###&lt;lineexpression&gt;
This element contains the style definition for line expressions (like line comments).  The ```type``` attribute can be used to specify the expression (as defined in the language xml file) to which the style will be applied. 
######Child element(s):
- &lt;color&gt;

###&lt;blockexpression&gt;
This element contains the style definition for block expressions (like block comments).  The ```type``` attribute can be used to specify the expression (as defined in the language xml file) to which the style will be applied. 
######Child element(s):
- &lt;color&gt;

###&lt;color&gt;
This is currently the only tag which actually defines a styling feature; color.  It encloses the color which will be used when highlighting
the token specified by the parent element.  The color can be specified in 3 different ways:

1. By specifying 3 or 4 numbers (between 0 and 255, separated by spaces) which represent the **R (red)**, **G (green)**, **B (blue)**, and **A (alpha/transparency, optional)** values which define the color.  eg. ```<color> 150 80 210 </color>```
2. By specifying the hex value of the color in one of the following formats:
  - #RGB
  - #RRGGBB -> most common
  - #AARRGGBB
  - #RRRGGGBBB
  - #RRRRGGGGBBBB
  - eg. ```<color> #008282 </color>```
3. By using the [SVG color name](http://www.w3.org/TR/SVG/types.html#ColorKeywords) provided by the World Wide Web Consortium.  
eg. ```<color> orange </color>```
