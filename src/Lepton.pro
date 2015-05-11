QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Lepton
TEMPLATE = app

CONFIG      += qscintilla2

SOURCES += main.cpp\
        mainwindow.cpp \
    editortabbar.cpp \
    scintillaeditor.cpp \
    leptonlexer.cpp \
    generalconfig.cpp \
    projectitem.cpp \
    syntaxhighlightmanager.cpp \
    settingseditor.cpp \
    findreplacedialog.cpp \
    leptonproject.cpp \
    projecttreeitem.cpp \
    projecttreemodel.cpp \
    projecttreeroot.cpp \
    projecttypemodel.cpp \
    projecttypeselector.cpp \
    loadprojectasdialog.cpp

HEADERS  += mainwindow.h \
    editortabbar.h \
    scintillaeditor.h \
    leptonlexer.h \
    generalconfig.h \
    projectitem.h \
    syntaxhighlightmanager.h \
    leptonconfig.h \
    settingseditor.h \
    findreplacedialog.h \
    leptonproject.h \
    projecttreeitem.h \
    projecttreemodel.h \
    projecttreeroot.h \
    projecttypemodel.h \
    projecttypeselector.h \
    loadprojectasdialog.h

FORMS    += mainwindow.ui \
    settingseditor.ui \
    findreplacedialog.ui \
    projecttypeselector.ui \
    loadprojectasdialog.ui

unix|win32: LIBS += -lqscintilla2
