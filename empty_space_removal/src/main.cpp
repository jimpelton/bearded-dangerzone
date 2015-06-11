
#include "binaryvolume.h"

// bd 
#include <bd/file/datareader.h>
#include <bd/scene/transformable.h>
#include <bd/util/util.h>

// simple_blocks 
#include <block.h>

// std/stl 
#include <functional>
#include <memory>
#include <iostream>
#include <iterator>

//////////////////////////////////////////////////////////////////////////
// 1. Create binary volume
// 2. Determine empty/non-empty regions from binary volume.
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
/// \brief An axis-aligned bounding box.
//////////////////////////////////////////////////////////////////////////
class AABB : public bd::Transformable
{
public:
    AABB();
    virtual ~AABB();
};


AABB::AABB(){ }


AABB::~AABB(){ }


struct TreeNode
{
    TreeNode() 
        : aabb{ }
        , leftNode{ nullptr }
        , rightNode{ nullptr }
        , depth{ 0 }
    {
    }

    virtual ~TreeNode()
    {
        if (leftNode){ delete leftNode; }
        if (rightNode){ delete rightNode; }
    }

    AABB aabb;
    TreeNode *leftNode;
    TreeNode *rightNode;

    size_t depth;

};

struct KDTree
{
    TreeNode *parent;
};



template<typename ValType>
struct IsEmptyFunc
{
    bool operator()(ValType v);
};

template<typename ValType>
bool IsEmptyFunc<ValType>::operator()
(
    ValType v
)
{
    if (v == static_cast<ValType>(0)) {
        return true;
    }

    return false;
}

int main(int argc, char *argv[])
{
    std::unique_ptr<float []> data = bd::readVolumeData(argv[2], argv[1], 32, 32, 32);
    size_t volSize = 32 * 32 * 32;
    std::vector<float> vecData(data.get(), data.get() + volSize);
    
    IsEmptyFunc<float> empty;
    BinVol<float> bv;
    bv.createBinaryVolume(vecData.begin(), vecData.end(), empty);

	return 0;
}
