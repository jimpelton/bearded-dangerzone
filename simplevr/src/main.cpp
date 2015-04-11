

#include "volrendloop.h"
#include "cmdline.h"

#include <util/context.h>

#include <iostream>




int main(int argc, char *argv[])
{
    CommandLineOptions opts;
    if (parseThem(argc, argv, opts) == 0) 
    {
        std::cout << "Check command line arguments... Exiting." << std::endl;
        return 1;
    } 

    VolRendLoop vr(opts);
    bd::Context *c = bd::Context::InitializeContext(&vr);
    bd::GlfwContext *gcon = reinterpret_cast<bd::GlfwContext*>(c);
    
    if (gcon == nullptr) {
        std::cout << "Downcast to GlfwContext failed." << std::endl;
        return 1;
    }

    //vr.initBlocks();

    vr.window(gcon->window());
    gcon->startLoop();

    return 0;
}