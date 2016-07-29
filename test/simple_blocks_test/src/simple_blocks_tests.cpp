#include <create_vao.h>
#include <axis_enum.h>
#include <constants.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <sstream>

#include <vector>
#include <array>

#include <catch.hpp>


namespace Catch {
std::string toString(const glm::vec3 &v) {
  return glm::to_string(v);
}

std::string toString(const glm::vec4 &v) {
  return glm::to_string(v);
}

std::string toString(const glm::u16vec4 &v) {
  return glm::to_string(v);
}

template<>
struct StringMaker<glm::vec4> {
  static std::string convert(const glm::vec4 &value) {
    return glm::to_string(value);
  }
};

} // namespace

TEST_CASE("createQuads_X single slice", "[vertex][X][single]") {
  glm::vec3 min{ 0, 0, 0 };
  glm::vec3 max{ 1, 1, 1 };
  float numSlices{ 1.0f };

  std::vector<glm::vec4> expected{
      glm::vec4{ 0, 0, 0, 1 },
      glm::vec4{ 0, 1, 0, 1 },
      glm::vec4{ 0, 0, 1, 1 },
      glm::vec4{ 0, 1, 1, 1 },
  };

  std::vector<glm::vec4> actual;
  subvol::createQuads(actual, min, max, numSlices, Axis::X);

  REQUIRE(actual == expected);
}


TEST_CASE("createQuads_X", "[vertex][X]") {
  glm::vec3 min{0, 0, 0};
  glm::vec3 max{1, 1, 1};
  float numSlices{4.0f};

  std::vector<glm::vec4> expected{
      glm::vec4{0, 0, 0, 1},
      glm::vec4{0, 1, 0, 1},
      glm::vec4{0, 0, 1, 1},
      glm::vec4{0, 1, 1, 1},

      glm::vec4{0.25, 0, 0, 1},
      glm::vec4{0.25, 1, 0, 1},
      glm::vec4{0.25, 0, 1, 1},
      glm::vec4{0.25, 1, 1, 1},

      glm::vec4{0.5, 0, 0, 1},
      glm::vec4{0.5, 1, 0, 1},
      glm::vec4{0.5, 0, 1, 1},
      glm::vec4{0.5, 1, 1, 1},

      glm::vec4{0.75, 0, 0, 1},
      glm::vec4{0.75, 1, 0, 1},
      glm::vec4{0.75, 0, 1, 1},
      glm::vec4{0.75, 1, 1, 1}

  };

  std::vector<glm::vec4> actual;
//  createQuads(actual, min, max, numSlices, Axis::X);
  subvol::createQuads(actual, min, max, numSlices, Axis::X);

  REQUIRE(actual == expected);

}

TEST_CASE("createQuads_Y", "[vertex][Y]") {

  glm::vec3 min{0, 0, 0};
  glm::vec3 max{1, 1, 1};
  float numSlices{4.0f};

  std::vector<glm::vec4> expected{
      glm::vec4{0, 0, 0, 1},
      glm::vec4{1, 0, 0, 1},
      glm::vec4{0, 0, 1, 1},
      glm::vec4{1, 0, 1, 1},

      glm::vec4{0, 0.25, 0, 1},
      glm::vec4{1, 0.25, 0, 1},
      glm::vec4{0, 0.25, 1, 1},
      glm::vec4{1, 0.25, 1, 1},

      glm::vec4{0, 0.5, 0, 1},
      glm::vec4{1, 0.5, 0, 1},
      glm::vec4{0, 0.5, 1, 1},
      glm::vec4{1, 0.5, 1, 1},

      glm::vec4{0, 0.75, 0, 1},
      glm::vec4{1, 0.75, 0, 1},
      glm::vec4{0, 0.75, 1, 1},
      glm::vec4{1, 0.75, 1, 1}

  };

  std::vector<glm::vec4> actual;
//  createQuads(actual, min, max, numSlices, Axis::Y);
  subvol::createQuads(actual, min, max, numSlices, Axis::Y);

  REQUIRE(actual == expected);

}

TEST_CASE("createQuads_Z", "[vertex][Z]") {

  glm::vec3 min{0, 0, 0};
  glm::vec3 max{1, 1, 1};
  float numSlices{4.0f};

  std::vector<glm::vec4> expected{
      glm::vec4{0, 0, 0, 1},
      glm::vec4{1, 0, 0, 1},
      glm::vec4{0, 1, 0, 1},
      glm::vec4{1, 1, 0, 1},

      glm::vec4{0, 0, 0.25, 1},
      glm::vec4{1, 0, 0.25, 1},
      glm::vec4{0, 1, 0.25, 1},
      glm::vec4{1, 1, 0.25, 1},

      glm::vec4{0, 0, 0.5, 1},
      glm::vec4{1, 0, 0.5, 1},
      glm::vec4{0, 1, 0.5, 1},
      glm::vec4{1, 1, 0.5, 1},

      glm::vec4{0, 0, 0.75, 1},
      glm::vec4{1, 0, 0.75, 1},
      glm::vec4{0, 1, 0.75, 1},
      glm::vec4{1, 1, 0.75, 1}

  };

  std::vector<glm::vec4> actual;
//  createQuads(actual, min, max, numSlices, Axis::Z);
  subvol::createQuads(actual, min, max, numSlices, Axis::Z);

  REQUIRE(actual == expected);

}

TEST_CASE("createElementIdx generates increasing sequence with restart symbol OxFFFF", "[vertex][elements]") {

  size_t numSlices{ 4 };
  std::vector<unsigned short> expected{
    0, 1, 2, 3, 0xFFFF,
    4, 5, 6, 7, 0xFFFF,
    8, 9, 10, 11, 0xFFFF,
    12, 13, 14, 15, 0xFFFF
  };

  std::vector<unsigned short> actual;
  subvol::createElementIdx(actual, numSlices);

  REQUIRE(actual == expected);

}


//TEST_CASE("sliceIndexToElements() returns triangle strip ordering", "[quad][vboIndexes]")
//{
//    std::vector<glm::u16vec4> ebuf_expected{
//        glm::u16vec4(0, 1, 3, 2),     // x-y
//        glm::u16vec4(4, 5, 7, 6),     // x-z
//        glm::u16vec4(8, 9, 11, 10)    // y-z
//    };
//
//    std::vector<glm::u16vec4> ebuf_actual;
//
//    int ebufIdx = 0;
//    for (int i = 0; i < 3; ++i) {
//        ebuf_actual.push_back(vert::sliceIndexToElements(ebufIdx));
//        ebufIdx++;
//    }
//
//    REQUIRE(ebuf_actual == ebuf_expected);
//}
//
//
//
//TEST_CASE("start returns 0.0 for one slice", "[quad][startCoords]")
//{
//    float expected = 0.0f;
//    float actual = vert::start(1, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//
//TEST_CASE("start returns -0.5  for two slices")
//{
//    float expected = -0.5f;
//    float actual = vert::start(2, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//
//TEST_CASE("start returns -0.5  for three slices")
//{
//    float expected = -0.5f;
//    float actual = vert::start(3, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//TEST_CASE("start returns -0.5  for four slices")
//{
//    float expected = -0.5f;
//    float actual = vert::start(4, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//
//TEST_CASE("start returns -0.5  for five slices")
//{
//    float expected = -0.5f;
//    float actual = vert::start(5, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//
//////////////////////////////////////////////////////////////////////////////////
//TEST_CASE("delta returns 0.0 for 1 slice")
//{
//    float expected = 0.0f;
//    float actual = vert::delta(1, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//
//TEST_CASE("delta returns 1.0 for 2 slices")
//{
//    float expected = 1.0f/2.0f;
//    float actual = vert::delta(2, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//
//TEST_CASE("delta returns 0.33 for 3 slices")
//{
//    float expected = 1.0f/3.0f;
//    float actual = vert::delta(3, -0.5f, 0.5f);
//
//    REQUIRE(actual == Approx(expected));
//}
//
//
//TEST_CASE("delta returns 0.25 for 4 slices")
//{
//    float expected = 1.0f/4.0f;
//    float actual = vert::delta(4, -0.5f, 0.5f);
//
//    REQUIRE(actual == Approx(expected));
//}
//
//
//TEST_CASE("delta returns 0.25 for 5 slices")
//{
//    float expected = 1.0f/5.0f;
//    float actual = vert::delta(5, -0.5f, 0.5f);
//
//    REQUIRE(actual == expected);
//}
//
//
//////////////////////////////////////////////////////////////////////////////////
//TEST_CASE("create_verts_xy creates 4 quads along z-axis.")
//{
//    float del = vert::delta(4, -0.5f, 0.5f);
//    std::vector<glm::vec4> vbuf;
//    vert::create_verts_xy(4, vbuf);
//
//    std::vector<glm::vec4> expected{
//        glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),  // 3 ul
//
//        glm::vec4(-0.5f, -0.5f, -0.5f + del, 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f + del, 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f + del, 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f + del, 1.0f),  // 3 ul
//
//        glm::vec4(-0.5f, -0.5f, -0.5f + (2 * del), 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f + (2 * del), 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f + (2 * del), 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f + (2 * del), 1.0f),  // 3 ul
//
//        glm::vec4(-0.5f, -0.5f, -0.5f + (3 * del), 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f + (3 * del), 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f + (3 * del), 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f + (3 * del), 1.0f)  // 3 ul
//    };
//
//    REQUIRE(16 == vbuf.size());
//    REQUIRE(vbuf == expected);
//}
//
//TEST_CASE("create_verts_xy creates 5 quads along z-axis.")
//{
//    float del = vert::delta(5, -0.5f, 0.5f);
//    std::vector<glm::vec4> vbuf;
//    vert::create_verts_xy(5, vbuf);
//
//    std::vector<glm::vec4> expected{
//        glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),  // 3 ul
//
//        glm::vec4(-0.5f, -0.5f, -0.5f + del, 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f + del, 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f + del, 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f + del, 1.0f),  // 3 ul
//
//        glm::vec4(-0.5f, -0.5f, -0.5f + (2 * del), 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f + (2 * del), 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f + (2 * del), 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f + (2 * del), 1.0f),  // 3 ul
//
//        glm::vec4(-0.5f, -0.5f, -0.5f + (3 * del), 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f + (3 * del), 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f + (3 * del), 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f + (3 * del), 1.0f),  // 3 ul
//
//        glm::vec4(-0.5f, -0.5f, -0.5f + (4 * del), 1.0f), // 0 ll
//        glm::vec4(0.5f, -0.5f, -0.5f + (4 * del), 1.0f), // 1 lr
//        glm::vec4(0.5f, 0.5f, -0.5f + (4 * del), 1.0f), // 2 ur
//        glm::vec4(-0.5f, 0.5f, -0.5f + (4 * del), 1.0f)  // 3 ul
//
//    };
//
//    REQUIRE(20 == vbuf.size());
//    REQUIRE(vbuf == expected);
//}
//
//
//////////////////////////////////////////////////////////////////////////////////
//TEST_CASE("create_texbuf_xy creates four slice tex coords z-axis.")
//{
//    float del = vert::delta(4, 0.0f, 1.0f);
//    std::vector<glm::vec4> vbuf;
//    vert::create_texbuf_xy(4, vbuf);
//
//    std::vector<glm::vec4> expected{
//        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // 3 ul
//
//        glm::vec4(0.0f, 0.0f, 0.0f + del, 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f + del, 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f + del, 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f + del, 1.0f),  // 3 ul
//
//        glm::vec4(0.0f, 0.0f, 0.0f + (2 * del), 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f + (2 * del), 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f + (2 * del), 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f + (2 * del), 1.0f),  // 3 ul
//
//        glm::vec4(0.0f, 0.0f, 0.0f + (3 * del), 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f + (3 * del), 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f + (3 * del), 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f + (3 * del), 1.0f)  // 3 ul
//    };
//
//    REQUIRE(16 == vbuf.size());
//    REQUIRE(vbuf == expected);
//}
//
//TEST_CASE("create_texbuf_xy creates five slice tex coords z-axis.")
//{
//    float del = vert::delta(5, 0.0f, 1.0f);
//    std::vector<glm::vec4> vbuf;
//    vert::create_texbuf_xy(5, vbuf);
//
//    std::vector<glm::vec4> expected{
//        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // 3 ul
//
//        glm::vec4(0.0f, 0.0f, 0.0f + del, 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f + del, 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f + del, 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f + del, 1.0f),  // 3 ul
//
//        glm::vec4(0.0f, 0.0f, 0.0f + (2 * del), 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f + (2 * del), 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f + (2 * del), 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f + (2 * del), 1.0f),  // 3 ul
//
//        glm::vec4(0.0f, 0.0f, 0.0f + (3 * del), 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f + (3 * del), 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f + (3 * del), 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f + (3 * del), 1.0f),  // 3 ul
//
//        glm::vec4(0.0f, 0.0f, 0.0f + (4 * del), 1.0f), // 0 ll
//        glm::vec4(1.0f, 0.0f, 0.0f + (4 * del), 1.0f), // 1 lr
//        glm::vec4(1.0f, 1.0f, 0.0f + (4 * del), 1.0f), // 2 ur
//        glm::vec4(0.0f, 1.0f, 0.0f + (4 * del), 1.0f)  // 3 ul
//    };
//
// //   REQUIRE(20 == vbuf.size());
//    REQUIRE(vbuf == expected);
//}
//
//
//////////////////////////////////////////////////////////////////////////////////
//
//TEST_CASE("create_elementIndices creates Triangle strip ordering.")
//{
//    std::vector<uint16_t> elebuf;
//    vert::create_elementIndices(4, elebuf);
//    std::vector<uint16_t> expected_buf{
//        0, 1, 3, 2,
//        0xFFFF,
//        4, 5, 7, 6,
//        0xFFFF,
//        8, 9, 11, 10,
//        0xFFFF,
//        12, 13, 15, 14,
//        0xFFFF
//    };
//
//    REQUIRE(elebuf == expected_buf);
//}
//
//TEST_CASE("create_elementIndicesReversed creates Triangle strip ordering.")
//{
//    std::vector<uint16_t> elebuf;
//    elebuf.resize(5*4);
//    auto b = elebuf.begin();
//    auto e = elebuf.end();
//    vert::create_elementIndicesReversed(4, b, e);
//    std::vector<uint16_t> expected_buf{
//        12, 13, 15, 14,
//        0xFFFF,
//        8, 9, 11, 10,
//        0xFFFF,
//        4, 5, 7, 6,
//        0xFFFF,
//        0, 1, 3, 2,
//        0xFFFF
//    };
//
//    REQUIRE(elebuf == expected_buf);
//}


//
//
//#include <iostream>
//#include <string>
//#include <array>
//#include <map>
//
//glm::mat4 identity{ 1.0 };
//
//
//const char* red = "\033[1;31m";
//const char* grn = "\033[1;32m";
//const char* rst = "\033[0m";
//
//std::map<std::string, bool> tests;
//
//#define TEST(x) \
//    do { \
//        std::cout << "=============================================" << std::endl; \
//        std::cout << "Running: " #x << std::endl; \
//        x(); \
//    } while (0)
//
//
//#define ASSERT_EQ(x,y) \
//    do { \
//        if ( (x) == (y) ) { \
//            std::cout << "{" << toString((x)) << "} == {" << toString((y)) << "}" << std::endl; \
//            std::cout << grn << "Pass!" << rst << std::endl; \
//            tests[__func__] = true; \
//        } else { \
//            std::cout << "{" << toString((x)) << "} != {" << toString((y)) << "}" << std::endl; \
//            std::cout << red << "Failed!" << rst << std::endl; \
//            tests[__func__] = false; \
//        } \
//    } while (0)
//
//std::string toString(unsigned short s)    { return std::to_string(s); }
//std::string toString( const glm::vec3 &v) { return glm::to_string(v); }
//std::string toString( const glm::vec4 &v) { return glm::to_string(v); }
//std::string toString( const glm::mat4 &v) { return glm::to_string(v); }
//
//void printTests()
//{
//    std::cout << "\n";
//    std::cout << "==========================================================="
//            "================" << std::endl;
//
//    for(auto &p : tests) {
//        const char *pf = "FAILED";
//        if (p.second)
//            pf = "PASSED!!";
//        std::cout << p.first << ": " << pf << "\n";
//    }
//
//    std::cout << std::endl;
//}
//
//void sliceIndexToElementsReturnsTriangleStripOrdering()
//{
//    std::vector<unsigned short> ebuf_expected{
//        0, 1, 3, 2,     // x-y
//        4, 5, 7, 6,     // x-z
//        8, 9, 11, 10    // y-z
//    };
//
//    std::vector<glm::u16vec4> ebuf;
//    for (int i = 0; i < 3; ++i) {
//        ebuf.push_back(sliceIndexToElements(i));
//    }
//
//    ASSERT_EQ(ebuf, ebuf_expected);
//
//}
//
//void singleSliceIsAtTheOrigin()
//{
//    std::vector<glm::vec4> vbuf;
//    
//}
//
//int main(int argc, char *argv)
//{
//    
//}
