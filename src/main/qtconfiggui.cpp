
#ifdef WITH_QT

#include "qtconfiggui.h"
#include "ui_qtconfiggui.h"


#include <iostream>

using namespace std;

bool getSettingsFromQtConfigGUI(int argc, char * argv[],
		Settings * settings) {
	bool correctClosed;
	QApplication a(argc, argv);
	QtConfigGUI w(settings, &correctClosed);
	w.show();
	a.exec();
	return correctClosed;
}

QtConfigGUI::QtConfigGUI(Settings * settings, bool * correctClosed, QWidget * parent) :
	QDialog(parent),
	m_settings(settings),
	m_correctClosed(correctClosed),
	ui(new Ui_QtConfigGUI) {
	*m_correctClosed = false;
	ui->setupUi(this);
}

QtConfigGUI::~QtConfigGUI() {
	delete ui;
}

void QtConfigGUI::changeEvent(QEvent *e) {
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void QtConfigGUI::on_comboBox_method_activated(QString element) {
	if (element != "Camera") {
		ui->pushButton_imagePath->setEnabled(true);
		ui->lineEdit_imagePath->setEnabled(true);
		ui->label_imagePath->setEnabled(true);
	} else {
		ui->pushButton_imagePath->setEnabled(false);
		ui->lineEdit_imagePath->setEnabled(false);
		ui->label_imagePath->setEnabled(false);
	}
}

void QtConfigGUI::on_pushButton_imagePath_clicked() {
	QString file = QFileDialog::getOpenFileName(this, tr("Select input file"),
			QDir::currentPath());
	ui->lineEdit_imagePath->setText(file);
}

void QtConfigGUI::on_pushButton_modelPath_clicked() {
	QString file = QFileDialog::getOpenFileName(this, tr("Select model"),
			QDir::currentPath());
	ui->lineEdit_modelPath->setText(file);
}

void QtConfigGUI::on_pushButton_printResults_clicked() {
	QString file = QFileDialog::getSaveFileName(this, tr("Save result file as"),
			QDir::currentPath());
	ui->lineEdit_printResults->setText(file);
}

void QtConfigGUI::on_pushButton_outputDir_clicked() {
	QString file = QFileDialog::getExistingDirectory(this, tr("Output directory"),
			QDir::currentPath());
	ui->lineEdit_outputDir->setText(file);
}

void QtConfigGUI::on_pushButton_printTiming_clicked() {
	QString file = QFileDialog::getSaveFileName(this, tr("Save timing file as"),
			QDir::currentPath());
	ui->lineEdit_printTiming->setText(file);
}

void QtConfigGUI::on_buttonBox_accepted() {
	vector<int> initialBoundingBox;

        if (!ui->lineEdit_imagePath->text().isEmpty()) {
		m_settings->m_imagePath = (ui->lineEdit_imagePath->text().toStdString().c_str());
	} else {
		m_settings->m_imagePath = "";
	}
        if (!ui->lineEdit_modelPath->text().isEmpty()) {
		m_settings->m_modelPath = (ui->lineEdit_modelPath->text().toStdString().c_str());
	} else {
		m_settings->m_modelPath = "";
	}
	if (!ui->lineEdit_outputDir->text().isEmpty()) {
		m_settings->m_outputDir = (ui->lineEdit_outputDir->text().toStdString().c_str());
	} else {
		m_settings->m_outputDir = "";
	}
        if ((!ui->lineEdit_printResults->text().isEmpty())&&ui->groupBox_printResults->isChecked()) {
		m_settings->m_printResults = (ui->lineEdit_printResults->text().toStdString());
	} else {
		m_settings->m_printResults = "";
	}
        if ((!ui->lineEdit_printTiming->text().isEmpty())&&ui->groupBox_printTiming->isChecked()) {
		m_settings->m_printTiming = (ui->lineEdit_printTiming->text().toStdString().c_str());
	} else {
		m_settings->m_printTiming = "";
	}

	if(ui->groupBox_initialBoundingBox->isChecked()) {
		initialBoundingBox.push_back(
				ui->lineEdit_initialBoundingBoxX->text().toInt());
		initialBoundingBox.push_back(
				ui->lineEdit_initialBoundingBoxY->text().toInt());
		initialBoundingBox.push_back(
				ui->lineEdit_initialBoundingBoxW->text().toInt());
		initialBoundingBox.push_back(
				ui->lineEdit_initialBoundingBoxH->text().toInt());
	}

	if(ui->comboBox_method->currentText() == "Camera") {
		m_settings->m_method = IMACQ_CAM;
	} else if(ui->comboBox_method->currentText() == "Video") {
		m_settings->m_method = IMACQ_VID;
	} else {
		m_settings->m_method = IMACQ_IMGS;
	}

	m_settings->m_useProportionalShift = ui->groupBox_proportionalShift->isChecked();
	m_settings->m_loadModel = ui->groupBox_loadModel->isChecked();
	m_settings->m_selectManually = ui->checkBox_selectManually->isChecked();
	m_settings->m_learningEnabled = ui->checkBox_learning->isChecked();
	m_settings->m_showOutput = ui->checkBox_showOutput->isChecked();
	m_settings->m_showNotConfident = ui->checkBox_showNotConfident->isChecked();
	m_settings->m_showColorImage = ui->checkBox_showColorImage->isChecked();
	m_settings->m_showDetections = ui->checkBox_showDetections->isChecked();
	m_settings->m_showForeground = ui->checkBox_showForeground->isChecked();
	m_settings->m_saveOutput = ui->groupBox_saveOutput->isChecked();
	m_settings->m_alternating = ui->checkBox_alternating->isChecked();
	m_settings->m_exportModelAfterRun = ui->checkBox_exportModel->isChecked();
	m_settings->m_trajectory = (ui->groupBox_trajectory->isChecked()) ? ui->lineEdit_trajectory->text().toInt() : 0;
	m_settings->m_startFrame = ui->lineEdit_startFrame->text().toInt();
	m_settings->m_lastFrame = (ui->groupBox_lastFrame->isChecked()) ? ui->lineEdit_lastFrame->text().toInt() : 0;
	m_settings->m_minScale = ui->lineEdit_minScale->text().toInt();
	m_settings->m_maxScale = ui->lineEdit_maxScale->text().toInt();
	m_settings->m_numFeatures = ui->lineEdit_numberOfFeatures->text().toInt();
	m_settings->m_numTrees = ui->lineEdit_numberOfTrees->text().toInt();
	m_settings->m_minSize = ui->lineEdit_minSize->text().toInt();
	m_settings->m_threshold = ui->lineEdit_theta->text().toDouble();
	m_settings->m_proportionalShift = ui->lineEdit_proportionalShift->text().toFloat();
	m_settings->m_initialBoundingBox = initialBoundingBox;

	*m_correctClosed = true;
	close();
}

void QtConfigGUI::on_buttonBox_rejected() {
	close();
}
;

#endif /* WITH_QT */
