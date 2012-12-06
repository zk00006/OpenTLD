/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/**
  * @author Clemens Korner
  */

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui/QApplication>
#include <QFileDialog>
#include <QDialog>

#include "ui_ConfigDialog.h"

namespace tld
{
class Settings;
}

/**
 * Creates a dialog and provides the configured parameters
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @param settings all parameters are stored in this object
 * @return true if everything went right, false if not
 */
bool getSettingsFromConfigDialog(int argc, char *argv[],
                                 tld::Settings *settings);

/**
 * Qt-Dialog to configure the program
 */
class ConfigDialog: public QDialog
{
    Q_OBJECT
public:
    /**
     * Creates a dialog
     * @param settings all parameters are stored in this object
     * @param correctClosed true if everything went right, false if not
     */
    ConfigDialog() {}
    ConfigDialog(tld::Settings *settings, bool *correctClosed, QWidget *parent = 0);
    ~ConfigDialog();

protected:
    void changeEvent(QEvent *e);

private:
    tld::Settings *m_settings;
    bool *m_correctClosed;
    Ui_ConfigDialog *ui;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_pushButton_printTiming_clicked();
    void on_pushButton_outputDir_clicked();
    void on_pushButton_printResults_clicked();
    void on_pushButton_modelPath_clicked();
    void on_pushButton_imagePath_clicked();
    void on_comboBox_method_activated(QString element);
};

#endif // CONFIGDIALOG_H
