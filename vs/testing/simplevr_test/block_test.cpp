
#include <block.h>

#include <gtest/gtest.h>

namespace 
{
    
    class Block_Test : public ::testing::Test 
    {
    public:
        Block_Test() { }
        virtual ~Block_Test() { }

        virtual void SetUp() override { }
        virtual void TearDown() override { }
    };
}

TEST_F(Block_Test, makeSlicesTest)
{
    //Block::makeBlockSlices(10);
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
