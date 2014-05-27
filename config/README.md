Lepton Configuration
====================
Most of the configuration for Lepton will be in this directory.  Currently, only theme config files are available.

Theme files
-----------
The file ``theme.conf`` defines general config information such as the path to the theme file used.  Some theme data is also located in 
this file, under the ``[Editor]`` heading, because it cannot be easily implemented in the actual theme file.

Theme files are simple cascading style sheet (CSS) files.  The default theme file is ``congif/themes/defaultTheme.css``.  To create a new
theme, you can copy the default file and edit the properties in the new file.  You must then edit ``config/theme.conf`` by setting
the value of the field ``theme`` (under the ``[MainWindow]`` heading) to the name of your new theme file. For example:
```
[MainWindow]
theme: newTheme.css
```
