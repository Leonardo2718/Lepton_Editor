Lepton Editor
=============

Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
flexible and extensible code editor which developers can easily customize to their
liking.  It is entirely programmed using C++ and Qt 5 framework, making it cross
platform and relatively lightweight.

At the moment, Lepton Editor is still in early alpha state.  I am actively developing
new features so it will be buggy.

### Current Features
- syntax highlighting for : 
  - C/C++
  - Python
  - Octave/Matlab
- auto indentation
- brace matching
- custom theme files
- saving sessions
- basic project manager

See the other README files to add new languages and highlighting styles.

### Features to Be:
- syntax highlighting for more languages:
  - Lua
  - Perl
  - Others (_See languages/README.md for documentation on how to create language files_)
- more editing options:
  - tabs vs spaces
  - indent space count
- code folding

Building and Running
--------------------

After cloning the repository, I recommend building Lepton Editor by opening the project
(.pro file) in Qt Creator (version 3.0.1 or later) and building (debugging) it from there.  The only
dependency for this build is QScintilla2 (http://www.riverbankcomputing.co.uk/software/qscintilla/intro).

If the build was successful, you should be able to just run the executable.  Make sure that the
**styles**, **languages**, and **config** directories are in the same path as the executable.  I recommend
creating symlinks/shortcuts to the directors in the path of the executable.

Contributing
------------

If you create new language/styling/theme files, please share those files!
Send me a pull request so I can add them with the other files.
