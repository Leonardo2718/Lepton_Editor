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
    projectmodel.cpp \
    projectitem.cpp \
    syntaxhighlightmanager.cpp \
    settingseditor.cpp \
    findreplacedialog.cpp

HEADERS  += mainwindow.h \
    editortabbar.h \
    scintillaeditor.h \
    leptonlexer.h \
    generalconfig.h \
    projectmodel.h \
    projectitem.h \
    syntaxhighlightmanager.h \
    leptonconfig.h \
    settingseditor.h \
    findreplacedialog.h

FORMS    += mainwindow.ui \
    settingseditor.ui \
    findreplacedialog.ui

unix|win32: LIBS += -lqscintilla2
