/*
Project: Lepton Editor
File: sessionmanager.h
Author: Leonardo Banderali
Created: October 20, 2015
Last Modified: October 28, 2015

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file defines a class used to manage user sessions.

Copyright (C) 2015 Leonardo Banderali

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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

// Qt classes
#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QDir>
#include <QMenu>
#include <QActionGroup>

// c++ standard libraries
#include <memory>



class SessionManager: public QObject {
    Q_OBJECT

    public:
        SessionManager();

        static void initSettings();
        /*  set the QSettings defaults */

        QMenu* selectionMenu();
        /*  returns the menu for selecting a user session */

        QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;
        /*  returns a setting value */

        void setValue(const QString & key, const QVariant & value);
        /*  sets a setting value */

    signals:
        void currentSessionChanged(QSettings* newSession, QSettings* oldSession);
        /*  emited when the current session is changed by the user */

        void aboutToChangeSession();    // emited when the current session is about to change
        void changedSession();          // emited when the current session has changed

    private:
        QSettings defaultSession;       // holds the default session information
        static std::unique_ptr<QSettings> currentSession;   // holds the current session information
        QDir sessionsDir;                                   // directory containing all the session config files
        std::unique_ptr<QMenu> sessionSelectionMenu;        // menu for letting the user select a session
        QActionGroup sessionActions;                        // actions for selecting a session

    private slots:
        void sessionChangeTriggered(QAction* sessionAction);
        /*  executed when a session action is triggered */
};

#endif // SESSIONMANAGER_H
