#include "settings.h"

using namespace std;

namespace tld {

Settings::Settings() :
		m_useProportionalShift(true),
		m_loadModel(false),
		m_selectManually(false),
		m_learningEnabled(true),
		m_showOutput(true),
		m_showNotConfident(true),
		m_showColorImage(false),
		m_showDetections(false),
		m_showForeground(false),
		m_saveOutput(false),
		m_alternating(false),
		m_exportModelAfterRun(false),
		m_trajectory(20),
		m_method(IMACQ_CAM),
		m_startFrame(1),
		m_lastFrame(0),
		m_minScale(-10),
		m_maxScale(10),
		m_numFeatures(10),
		m_numTrees(10),
		m_minSize(25),
		m_threshold(0.7),
		m_proportionalShift(0.1),
		m_modelExportFile("model"),
		m_initialBoundingBox(vector<int>()) {
}

Settings::~Settings() {
}

}
