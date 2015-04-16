#include <bd/util/transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <string>
#include <map>

glm::mat4 identity{ 1.0 };

using namespace bd;

const char* red = "\033[1;31m";
const char* grn = "\033[1;32m";
const char* rst = "\033[0m";

std::map<std::string, bool> tests;

#define TEST(x) \
    do { \
        std::cout << "=============================================" << std::endl; \
        std::cout << "Running: " #x << std::endl; \
        x(); \
    } while (0)


#define ASSERT_EQ(x,y) \
    do { \
        if ( (x) == (y) ) { \
            std::cout << "{" << toString((x)) << "} == {" << toString((y)) << "}" << std::endl; \
            std::cout << grn << "Pass!" << rst << std::endl; \
            tests[__func__] = true; \
        } else { \
            std::cout << "{" << toString((x)) << "} != {" << toString((y)) << "}" << std::endl; \
            std::cout << red << "Failed!" << rst << std::endl; \
            tests[__func__] = false; \
        } \
    } while (0)

std::string toString( const glm::vec3 &v)
{
    return glm::to_string(v);
}

std::string toString( const glm::vec4 &v)
{
    return glm::to_string(v);
}

std::string toString( const glm::mat4 &v)
{
    return glm::to_string(v);
}



void printTests()
{
    std::cout << "\n";
    std::cout << "==========================================================="
            "================" << std::endl;

    for(auto &p : tests) {
        const char *pf = "FAILED";
        if (p.second)
            pf = "PASSED!!";
        std::cout << p.first << ": " << pf << "\n";
    }

    std::cout << std::endl;
}

//////////////////////////////////////////////////////////////////////////////
void defaultConstructedAtOrigin()
{
    Transform tr;
    ASSERT_EQ(tr.matrix(), glm::mat4(1.0));
}


//////////////////////////////////////////////////////////////////////////////
void positionSetsPosVector()
{
    Transform tr;
    tr.position(glm::vec3(1.f, 0.f, 0.f));
    ASSERT_EQ(tr.position(), glm::vec3(1.f, 0.f, 0.f));
}


//////////////////////////////////////////////////////////////////////////////
void scaleSetsScaleVector()
{
    Transform tr;
    tr.scale(glm::vec3(1.f, 0.f, 0.f));
    ASSERT_EQ(tr.scale(), glm::vec3(1.f, 0.f, 0.f));
}


//////////////////////////////////////////////////////////////////////////////
void orentationSetsRotationQuat()
{
    Transform tr;
    tr.orientation(glm::quat());
}

void rotateDoesWhatever()
{
    Transform tr;
    glm::quat q(M_PI_4, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 v = q * glm::vec3(0.0f, 0.0f, 1.0f);

//    tr.rotate(M_PI_4, glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::vec3 v = tr.rotation() * glm::vec3(0.0f, 0.0f, 1.0f);

    ASSERT_EQ(v, glm::vec3(1.0f, 0.0f, 0.0f));

}

void rotateAccumulatesRotation()
{
    Transform tr;
    tr.rotate(M_PI_4, glm::vec3(0.0f, 1.0f, 0.0f));
    tr.rotate(M_PI_4, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 v = tr.rotation() * glm::vec3(0.0f, 0.0f, 1.0f);
    ASSERT_EQ(glm::normalize(v), glm::vec3(1.0f, 0.0f, 0.0f));
}

int main(int argc, char *argv[])
{
    TEST(defaultConstructedAtOrigin);
    TEST(positionSetsPosVector);
    TEST(scaleSetsScaleVector);
    TEST(orentationSetsRotationQuat);
    TEST(rotateDoesWhatever);
    TEST(rotateAccumulatesRotation);

    printTests();

    return 0;
}
