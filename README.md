Lepton Editor
=============

Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
flexible and extensible code editor which developers can easily customize to their
liking.  It is entirely programmed using C++ and Qt 5.2.1 framework, making it cross
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
- saving sessions
- basic project manager

Building and Running
--------

After having cloned the repository, you can build Lepton Editor by either opening the project
(.pro file) in Qt Creator (version 3.0.1 or later) and build it from there or you can manually
build the .pro file using qmake 5.2.1 or later.  The only dependency for this build is
QScintilla2: http://www.riverbankcomputing.co.uk/software/qscintilla/intro

If the build was successful, you should be able to just run the executable.  Make sure that the
'styles' and 'languages' directories are in the same path as the executable.  If they are not,
Lepton should run fine but you will not have any syntax highlighting options.  I recommend
creating a link to the two directors in the path of the executable.  This way, you can conserve
the directory tree and not have to worry about file duplicates.

Contributing
------------

If you create new language/styling files, I would appreciate it if you would share those files.
Send me a pull request so I can add them to the other files.
