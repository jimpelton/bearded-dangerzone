
//////////////////////////////////////////////////////////////////////////////
// SimpleVr
//
//////////////////////////////////////////////////////////////////////////////

#include "cmdline.h"
#include "volrendloop.h"
#include "geometry.h"
#include "blockscollection.h"
#include "block.h"

#include <bd/util/boundingbox.h>
#include <bd/graphics/quad.h>
//#include <bd/graphics/BBox.h>

#include <iostream>
#include <algorithm>


//const std::vector<glm::vec3> qcolors{
//    { 0.0, 0.0, 0.0 },
//    { 1.0, 0.0, 0.0 },
//    { 0.0, 1.0, 0.0 },
//    { 0.0, 0.0, 1.0 }
//};

void makeBlocks(BlocksCollection &bc,
    size_t bx, size_t by, size_t bz,
    size_t vx, size_t vy, size_t vz)
{
    bc.initBlocks(glm::u64vec3{bx,by,bz}, glm::u64vec3{vx,vy,vz});
}


int main(int argc, const char *argv[])
{
    CommandLineOptions opts;
    if (parseThem(argc, argv, opts) == 0) {
        std::cout << "Check command line arguments... Exiting." << std::endl;
        return 1;
    }
    printThem(opts);

    VolRendLoop vr;
    bd::Context *c = bd::Context::InitializeContext(&vr);
    if (c == nullptr) {
        std::cout << "Context initialization failed." << std::endl;
        exit(1);
    }

    vr.makeVolumeRenderingShaders("", "");

//    std::vector<glm::vec4> vertices;
//    std::vector<unsigned short> indices;
//    makeBlockSlices(opts.num_slices, vertices, indices);

//    BlocksCollection bc;
//    makeBlocks(bc, opts.block_side, opts.block_side, opts.block_side,
//        opts.w, opts.h, opts.d);

//    bd::Transformable root;
//    auto blocks = bc.blocks();
//    std::for_each(blocks.begin(), blocks.end(),
//        [&root](auto &b) { root.addChild(&b); } );


//    std::vector<glm::vec4> vertices(bd::Box::vertices.begin(), bd::Box::vertices.end());
//    std::vector<unsigned short> indices(bd::Box::elements.begin(), bd::Box::elements.end());
    std::vector<glm::vec3> colors(bd::Quad::colors.begin(), bd::Quad::colors.end());
    std::vector<glm::vec4> vertices(bd::Quad::verts.begin(), bd::Quad::verts.end());
    std::vector<unsigned short> indices(bd::Quad::elements.begin(), bd::Quad::elements.end());
    bd::Transformable root;
    vr.root(&root);
    vr.addVbaContext(vertices, colors, indices);

    c->startLoop();

    return 0;
}
