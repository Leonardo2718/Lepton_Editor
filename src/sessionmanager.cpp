/*
Project: Lepton Editor
File: sessionmanager.cpp
Author: Leonardo Banderali
Created: October 20, 2015
Last Modified: October 29, 2015

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

#include <QDebug>

//~public SessionManager member implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SessionManager::SessionManager() : QObject(0), defaultSession{"/Lepton Editor/default"},
    sessionActions{0} {
    sessionsDir.setPath(LeptonConfig::mainSettings->getConfigDirPath("sessions/Lepton Editor"));
    sessionSelectionMenu = std::make_unique<QMenu>();
    loadSessionActions();
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
QVariant SessionManager::value(const QString & key, const QVariant & defaultValue) {
    auto sessionString = defaultSession.value("__session").toString();
    if (sessionString.isEmpty()) {
        auto v = defaultSession.value(key, defaultValue);
        return v;
    } else {
        QSettings s{sessionString.prepend("/Lepton Editor/")};
        return s.value(key, defaultValue);
    }
}

/*
sets a setting value
*/
void SessionManager::setValue(const QString & key, const QVariant & value) {
    auto sessionFilePath = defaultSession.value("__session").toString();
    if (sessionFilePath.isEmpty()) {
        defaultSession.setValue(key, value);
    } else {
        QSettings s{sessionFilePath.prepend("/Lepton Editor/")};
        s.setValue(key, value);
    }
}



//~private function implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
loads the actions for selecting a session
*/
void SessionManager::loadSessionActions() {
    foreach (QAction* a, sessionSelectionMenu->actions()) {
        sessionActions.removeAction(a);
    }
    sessionSelectionMenu->clear();
    QStringList nameFilters;
    nameFilters.append(QString{"*.conf"});
    foreach (const QFileInfo& sessionFile, sessionsDir.entryInfoList(nameFilters, QDir::AllEntries | QDir::NoDotAndDotDot)) {
        auto action = sessionActions.addAction(sessionFile.baseName());
        action->setCheckable(true);
        auto session = defaultSession.value("__session").toString();
        if ((sessionFile.baseName() == session) || (sessionFile.baseName() == "default" && session.isEmpty())) {
            action->setChecked(true);
        }
    }
}



//~private SessionManager slot implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
executed when a session action is triggered
*/
void SessionManager::sessionChangeTriggered(QAction* sessionAction) {
    emit this->aboutToChangeSession();

    if (sessionAction->text() == "default") {
        defaultSession.setValue("__session", QVariant{});
    }
    else {
        QString text = sessionAction->text();
        defaultSession.setValue("__session", QVariant{text});
        bool b = defaultSession.value("__Session").toString() == text;
    }

    emit this->changedSession();
}
