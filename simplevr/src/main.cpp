
#include "cmdline.h"
#include "volrendloop.h"
#include "geometry.h"

#include <iostream>


#ifndef VR_SHADER_PATH
#define VR_SHADER_PATH
#endif

int main(int argc, const char *argv[])
{
    CommandLineOptions opts;
    if (parseThem(argc, argv, opts) == 0) {
        std::cout << "Check command line arguments... Exiting." << std::endl;
        return 1;
    }
    printThem(opts);

    VolRendLoop vr(opts);
    bd::Context *c = bd::Context::InitializeContext(&vr);

    if (c == nullptr) {
        std::cout << "Context initialization failed." << std::endl;
        return 1;
    }


    vr.makeVolumeRenderingShaders("shaders/simple-vs.glsl",
                                  "shaders/simple-color-frag.glsl");

    std::vector<glm::vec4> vertices;
    std::vector<unsigned short> indices;
    makeBlockSlices(opts.num_slices, vertices, indices);
    vr.addVbaContext(vertices, indices);

    c->startLoop();

    return 0;
}
