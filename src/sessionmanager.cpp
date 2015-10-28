/*
Project: Lepton Editor
File: sessionmanager.cpp
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

// project headers
#include "sessionmanager.h"
#include "leptonconfig.h"



//~public SessionManager member implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SessionManager::SessionManager() : QObject(0), defaultSession{LeptonConfig::mainSettings->getConfigDirPath("sessions").append("/default.conf")},
    sessionActions{0} {
    currentSession.release();
    currentSession = std::make_unique<QSettings>(LeptonConfig::mainSettings->getConfigDirPath("sessions").append("/sessions"));
    sessionSelectionMenu = std::make_unique<QMenu>();
    sessionActions.addAction("default");
    sessionSelectionMenu->addActions(sessionActions.actions());
    connect(&sessionActions, SIGNAL(triggered(QAction*)), this, SLOT(sessionChangeTriggered(QAction*)));
}

/*
set the QSettings defaults
*/
void SessionManager::initSettings() {
    QSettings::setDefaultFormat(QSettings::NativeFormat);   // I may decide to change this later on and use my own format
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, LeptonConfig::mainSettings->getConfigDirPath("sessions"));
}

/*
returns the menu for selecting a user session
*/
QMenu* SessionManager::selectionMenu() {
    return sessionSelectionMenu.get();
}

/*
returns a setting value
*/
QVariant SessionManager::value(const QString & key, const QVariant & defaultValue) const {
    if (currentSession)
        return currentSession->value(key, defaultValue);
    else
        return defaultSession.value(key, defaultValue);
}

/*
sets a setting value
*/
void SessionManager::setValue(const QString & key, const QVariant & value) {
    if (currentSession)
        currentSession->setValue(key, value);
    else
        defaultSession.setValue(key, value);
}



//~private SessionManager slot implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
executed when a session action is triggered
*/
void SessionManager::sessionChangeTriggered(QAction* sessionAction) {
    //emit currentSessionChanged(new QSettings{}, new QSettings{});
    emit this->aboutToChangeSession();

    if (sessionAction->text() == "default") {
        currentSession.release();
        defaultSession.value("session", QVariant{});
    }
    else {
        QString sessionFilPath = LeptonConfig::mainSettings->getConfigDirPath("sessions").append("/sessions/").append(sessionAction->text());
        currentSession.release();
        currentSession = std::make_unique<QSettings>(sessionFilPath);
        defaultSession.value("session", QVariant{sessionAction->text()});
    }

    emit this->changedSession();
}
