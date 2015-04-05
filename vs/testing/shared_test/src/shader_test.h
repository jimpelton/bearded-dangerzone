#ifndef shader_test_h__
#define shader_test_h__

#include <gtest/gtest.h>

namespace {
    class Shader_Test : public ::testing::Test {
    public:
        Shader_Test() { }
        virtual ~Shader_Test() { }

        virtual void SetUp() override { }
        virtual void TearDown() override { }
    };
}


#endif
