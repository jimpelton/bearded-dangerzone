
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <bd/util/transform.h>
#include <bd/util/transformable.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>


namespace Catch {
    inline std::string toString( const glm::vec3 &v ) {
        return glm::to_string(v);
    }

    inline std::string toString( const glm::vec4 &v ) {
        return glm::to_string(v);
    }

    inline std::string toString( const glm::mat4 &v ) {
        return glm::to_string(v);
    }
}

glm::mat4 identity { 1.0f };
glm::vec3 origin {0.0f, 0.0f, 0.0f};

TEST_CASE("a transformable is constructed at the origin", "[transformable][origin]") 
{
        bd::Transformable tr;
        tr.update();
        REQUIRE( tr.transform().matrix() == identity );
        REQUIRE( tr.transform().position() == origin);
}

