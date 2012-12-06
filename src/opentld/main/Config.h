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

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>

#include <libconfig.h++>

#include "ImAcq.h"
#include "Settings.h"
#include "Main.h"

namespace tld
{

/**
 * Config is used to configure the program by cli and/or by a config file
 */
class Config
{
public:
    /**
     * Constructor
     */
    Config();

    Config(Settings &settings);

    /**
     * Destructor
     */
    ~Config();

    /**
     * Initializes the Config.
     * @param argc number of command line arguments
     * @param argv command line arguments
     * @return PROGRAM_EXIT when an error occurred, SUCCESS if not
     */
    int init(int argc, char **argv);

    /**
     * Configures the CamNode, ImAcq and TldClassifier.
     * @param node
     * @param imAcq
     * @param classifier
     * @return SUCCESS
     */
    int configure(Main *main);
private:
    libconfig::Config m_cfg; //!< libconfig++
    std::string m_configPath; //!< path to the config file
    Settings m_settings; //!< adjusted settings
    ///@{
    /**
     * Flags if parameters were set by cli-arguments.
     * When the flags are set, the parameters couldn't be
     * overwritten by the options in the config file.
     * The cli-arguments have a higher priority than the options
     * in the config file.
     */
    bool m_selectManuallySet;
    bool m_methodSet;
    bool m_startFrameSet;
    bool m_lastFrameSet;
    bool m_trajectorySet;
    bool m_showDetectionsSet;
    bool m_showForegroundSet;
    bool m_thetaSet;
    bool m_printResultsSet;
    bool m_camNoSet;
    bool m_imagePathSet;
    bool m_modelPathSet;
    bool m_initialBBSet;
    bool m_showOutputSet;
    bool m_exportModelAfterRunSet;
    ///@}
};


/*
 POSIX getopt for Windows

 AT&T Public License

 Code given out at the 1985 UNIFORUM conference in Dallas.
 */
#ifdef __GNUC__
#   include <getopt.h>
#endif

#ifndef __GNUC__
extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
extern int getopt(int argc, char **argv, char *opts);
#endif  /* __GNUC__ */

} /* End Namespace */


#endif /* CONFIG_H */
