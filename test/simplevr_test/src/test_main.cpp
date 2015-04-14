//
// Created by jim on 4/13/15.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <geometry.h>
#include <cmdline.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>


void makeMeACloBro(int numslices, CommandLineOptions &clo)
{
    clo.filePath = std::string("");
    clo.type = std::string("");
    clo.block_side = 3;
    clo.num_slices = numslices;
    clo.w = 32;
    clo.h = 32;
    clo.d = 32;
}

TEST_CASE( "geometry makes slices", "[makeSlices]" ) {

    std::vector<glm::vec4> vertices;
    std::vector<unsigned short> indices;
    int num_slices = 0;

    SECTION( "Makes at least one slice" ) {

        num_slices = 0;

        makeBlockSlices(num_slices, vertices, indices);

        REQUIRE(vertices.size() == 4);
        REQUIRE(indices.size() == 5);

    }

    SECTION( "Makes exactly one slice when given one" ) {

        num_slices = 1;

        makeBlockSlices(num_slices, vertices, indices);

        REQUIRE(vertices.size() == 4);
        REQUIRE(indices.size() == 5);
    }

    SECTION( "Makes an even number of slices when given an odd > 1") {

        num_slices = 3;

        makeBlockSlices(num_slices, vertices, indices);

        REQUIRE(vertices.size() == (num_slices+1)*4);
        REQUIRE(indices.size() == (num_slices+1)*5);
    }

    SECTION( "Slice vertices are in the correct order with four slices") {

        num_slices = 4;

        makeBlockSlices(num_slices, vertices, indices);

        for(size_t i=0; i<vertices.size(); i+=4) {
            REQUIRE( vertices[i]   == glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f) );
            REQUIRE( vertices[i+1] == glm::vec4(0.5f, -0.5f, 0.0f, 1.0f)  );
            REQUIRE( vertices[i+2] == glm::vec4(0.5f, 0.5f, 0.0f, 1.0f)   );
            REQUIRE( vertices[i+3] == glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)  );
        }
    }

    SECTION( "Slice indicies are in correct order with four slices" ) {
        num_slices = 4;
        makeBlockSlices(num_slices, vertices, indices);

        for(size_t i=0; i<indices.size(); i+=5) {
            REQUIRE( indices[i]   == 0);
            REQUIRE( indices[i+1] == 1);
            REQUIRE( indices[i+2] == 3);
            REQUIRE( indices[i+3] == 2);
            REQUIRE( indices[i+4] == 0xFFFF );
        }
    }
}

