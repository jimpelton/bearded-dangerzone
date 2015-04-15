#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <bd/util/transform.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

glm::mat4 identity{ 1.0 };

SCENARIO("A translation combines rigid transforms", "[combineRigidTransforms]")
{
    GIVEN("A default constructed transform") {
        bd::Transform tr;

        REQUIRE(tr.matrix() == glm::mat4(1.0));

        WHEN("position changed to <1,1,1>") {
            tr.position(glm::vec3(1.0, 1.0, 1.0));
            tr.update(identity);

            //std::cout << glm::to_string(tr.matrix()) << std::endl;

            THEN(" 4th column should be <1,1,1,1>") {
                glm::mat4 expected {
                    glm::vec4(1,0,0,0),
                    glm::vec4(0,1,0,0),
                    glm::vec4(0,0,1,0),
                    glm::vec4(1,1,1,1)
                };

                REQUIRE (tr.matrix() == expected);
            }
        }

        WHEN("translated by <-0.5,-0.5,-0.5> starting at <1,1,1>") {
            tr.position(glm::vec3(1,1,1));
            tr.translate(glm::vec3(-0.5f, -0.5f, -0.5f));
            tr.update(identity);

            //std::cout << glm::to_string(tr.matrix()) << std::endl;

            THEN("4th column should be <0.5,0.5,0.5,1>.") {
                glm::mat4 expected {
                    glm::vec4(1,0,0,0),
                    glm::vec4(0,1,0,0),
                    glm::vec4(0,0,1,0),
                    glm::vec4(0.5,0.5,0.5,1)
                
                };

                REQUIRE(tr.matrix() == expected);
            }
        }

        WHEN("updated with a non-identity parent patrix") {
            //tr.position(glm::vec3(1,1,1));
            tr.translate(glm::vec3(-0.5f, -0.5f, -0.5f));
            tr.update(
                glm::mat4 {
                    glm::vec4(1,0,0,0),
                    glm::vec4(0,1,0,0),
                    glm::vec4(0,0,1,0),
                    glm::vec4(1,1,1,1)
                }
            );
            
            THEN("matrix includes parent transforms") {
                glm::mat4 expected {
                    glm::vec4(1,0,0,0),
                    glm::vec4(0,1,0,0),
                    glm::vec4(0,0,1,0),
                    glm::vec4(0.5,0.5,0.5,1)
                };

                REQUIRE(tr.matrix() == expected);
            }
        }
    }
}
