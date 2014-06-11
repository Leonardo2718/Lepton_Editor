/*
Project: Lepton Editor
File: findreplacedialog.h
Author: Leonardo Banderali
Created: December 5, 2014
Last Modified: December 20, 2014

Description:
    Lepton Editor is a text editor oriented towards programmers.  It's intended to be a
    flexible and extensible code editor which developers can easily customize to their
    liking.

    This file defines the `FindReplaceDialog` class.  This class provides a GUI for
    doing find/replace tasks on a file being edited.

Copyright (C) 2014 Leonardo Banderali

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

#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H


//include Qt classes
#include <QWidget>


//define namespaces
namespace Ui {
    class FindReplaceDialog;
}



//define class
class FindReplaceDialog : public QWidget {

    Q_OBJECT

    public:
        explicit FindReplaceDialog(QWidget *parent = 0);
        ~FindReplaceDialog();

        typedef struct paramater_data {
                QString findText;
                QString replaceText;
                bool isRegex;
                bool caseSensitive;
                bool matchWholeWord;
                bool wrap;
                bool forwardSearch;
        } DialogParameters;

        const DialogParameters& getDialogParameters();
        /*  -returns the parameters selected in the dialog */

    signals:
        void findClicked(const FindReplaceDialog::DialogParameters&);
        /*  -emited to signal other objects that `Find` has been clicked
            -the only argument is the dialog parameters when the button was clicked
        */

        void findNextClicked(const FindReplaceDialog::DialogParameters&);
        /*  -emited to signal other objects that `Find Next` has been clicked */

        void replaceClicked(const FindReplaceDialog::DialogParameters&);
        /*  -emited to signal other objects that `Replace` has been clicked */

    private slots:
        void on_findButton_clicked();
        /*  -called when the `Find` button is clicked */

        void on_findNextButton_clicked();
        /*  -called when the `Find Next` button is clicked */

        void on_replaceButton_clicked();
        /*  -called when the `Replace` button is clicked */

        void on_cancelButton_clicked();
        /*  -called when the `Cancel` button is clicked */


    private:
        Ui::FindReplaceDialog *ui;  //GUI object
        DialogParameters params;    //used to store the parameters of the dialog

        void storeParams();
        /*  -read paramaters from the dialog and store their values */
};


#endif // FINDREPLACEDIALOG_H
