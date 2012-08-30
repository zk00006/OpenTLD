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

#include <cstdio>

#include "Main.h"
#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "ConfigDialog.h"

using tld::Config;
using tld::Gui;
using tld::Settings;

int main(int argc, char **argv)
{

    Main *main = new Main();
    ImAcq *imAcq = imAcqAlloc();
    Gui *gui = new Gui();
    Settings *settings = new Settings();

    main->gui = gui;
    main->imAcq = imAcq;

    if(!getSettingsFromConfigDialog(argc, argv, settings))
    {
        return EXIT_FAILURE;
    }

    fprintf(stdout, "OpenTLD appeares soon...\n");

    Config config(*settings);
    config.configure(main);

    srand(main->seed);

    imAcqInit(imAcq);

    if(main->showOutput)
    {
        gui->init();
    }

    main->doWork();

    delete main;

    return EXIT_SUCCESS;
}
