Lepton Editor
=============

Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
flexible and extensible code editor which developers can easily customize to their
liking.  It is entirely programmed using C++ and Qt 5.2.1 framework, making it cross
platform and relatively lightweight.

At the moment, Lepton Editor is still in early alpha state.  I am actively developing
new features so it will be a little buggy.  However, I will do my best to make it
stable enough so its at least usable.

### Current Features
- syntax highlighting for : 
  - C
  - C++

See the other README files to add new languages and highlighting styles.

### Features to Be:
- syntax highlighting for more languages:
  - Python
  - Octave/Matlab
  - Lua
  - Perl
  - Others (_See languages/README.md for documentation on how to create language files_)
- more editing options:
  - tabs vs spaces
  - indent space count
  - auto indentation.
- bracket matching
- code folding
- saving sessions
- basic project manager

Building
--------

After having cloned the repository, you can build Lepton Editor by either opening the project
(.pro file) in Qt Creator 3.0.1 and build it from there or you can manually build the .pro
file using qmake 5.2.1 .

Contributing
------------

If you would like Lepton Editor to support a particular language, please consider creating and
contributing the language file yourself.
