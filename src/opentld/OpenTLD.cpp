#include "Main.h"
#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"

using namespace tld;

int main(int argc, char **argv)
{

    Main *main = new Main();
    Config config;
    ImAcq *imAcq = imAcqAlloc();
    Gui *gui = new Gui();

    main->gui = gui;
    main->imAcq = imAcq;

    if(config.init(argc, argv) == PROGRAM_EXIT)
    {
        return EXIT_FAILURE;
    }

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
