#ifndef quad_test_h__
#define quad_test_h__

#include <gtest/gtest.h>

namespace {
    class Quad_Test : public ::testing::Test {
    public:
        Quad_Test() { }
        virtual ~Quad_Test() { }

        virtual void SetUp() override { }
        virtual void TearDown() override { }
    };
}

#endif
