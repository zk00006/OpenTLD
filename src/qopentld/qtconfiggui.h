#ifdef WITH_QT

#ifndef QTCONFIGGUI_H
#define QTCONFIGGUI_H

#include <QtGui/QApplication>
#include <QFileDialog>
#include <QDialog>

#include "settings.h"

#include "ui_qtconfiggui.h"

/**
 * Creates a dialog and provides the configured parameters
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @param settings all parameters are stored in this object
 * @return true if everything went right, false if not
 */
bool getSettingsFromQtConfigGUI(int argc, char * argv[],
		tld::Settings * settings);

/**
 * Qt-Dialog to configure the program
 */
class QtConfigGUI: public QDialog {
Q_OBJECT
public:
	/**
	 * Creates a dialog
	 * @param settings all parameters are stored in this object
	 * @param correctClosed true if everything went right, false if not
	 */
	QtConfigGUI() {}
	QtConfigGUI(tld::Settings * settings, bool * correctClosed, QWidget * parent = 0);
	~QtConfigGUI();

protected:
	void changeEvent(QEvent *e);

private:
	tld::Settings * m_settings;
	bool * m_correctClosed;
	Ui_QtConfigGUI *ui;

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

#endif // QTCONFIGGUI_H

#endif /* WITH_QT */
