
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <bd/util/transform.h>
#include <bd/util/transformable.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/detail/precision.hpp>
#include <glm/detail/type_mat.hpp>

#include <iostream>


namespace Catch {
    std::string toString( const glm::vec3 &v ) {
        return glm::to_string(v);
    }

    std::string toString( const glm::vec4 &v ) {
        return glm::to_string(v);
    }

    std::string toString( const glm::mat4 &v ) {
        return glm::to_string(v);
    }

//   std::string toString( const glm::tmat4x4<float, glm::highp> &v ) {
//       return glm::to_string(v);
//   }
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

TEST_CASE("a child is relative to its parent", "[transformable][children]")
{
    bd::Transformable tr;
    bd::Transformable tr_child;

    tr.addChild(&tr_child);
    tr.update();
    REQUIRE(tr.transform().matrix() == tr_child.transform().matrix());

    tr.translate(glm::vec3(1.0f, 0.0f, 0.0f));
    tr.update();
    REQUIRE(tr.transform().matrix()[3] == glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    std::cout << "child should be at 1.0,0.0,0.0: " << 
        glm::to_string(tr_child.transform().matrix()) << "\n";

    REQUIRE(tr_child.transform().matrix()[3] == glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    tr_child.translate(glm::vec3( 0.5f, 0.0f, 0.0f ));
    tr.update();
    std::cout << "child should be at 1.5,0.0,0.0: " << 
        glm::to_string(tr_child.transform().matrix()) << "\n";

    REQUIRE(tr_child.transform().matrix()[3] == glm::vec4(1.5f, 0.0f, 0.0f, 1.0f));

}
