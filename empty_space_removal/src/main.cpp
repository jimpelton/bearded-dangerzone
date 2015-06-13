
#include "binaryvolume.h"

// bd 
#include <bd/file/datareader.h>
#include <bd/scene/transformable.h>
#include <bd/util/util.h>

// simple_blocks 
#include <block.h>
#include <cmdline.h>

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

class KDTree
{
public:
    KDTree();
    virtual ~KDTree();

    TreeNode *root;

};

KDTree::KDTree() 
    : root{ nullptr }
{
    
}

KDTree::~KDTree()
{
    
}



template<typename ValType>
struct IsEmptyFunc
{
    bool operator()(ValType v);

    ValType tmin;
    ValType tmax;
};


template<typename ValType>
bool IsEmptyFunc<ValType>::operator() (ValType v)
{
    if (v == static_cast<ValType>(0)) {
        return true;
    }

    return false;
}


void usage()
{
    std::cout << "<file-name> <data-type> <vol-dims (one number)>" << std::endl;
}


int main(int argc, const char *argv[])
{
    CommandLineOptions opts;
    if (parseThem(argc, argv, opts) == 0){ return; }

    size_t dim = atoll(argv[3]);
    std::unique_ptr<float []> data = bd::readVolumeData(argv[2], argv[1], dim, dim, dim);
    size_t volSize = dim*dim*dim;
    
    IsEmptyFunc<float> empty;
    BinVolFromArray<float> bv;
    bv.createBinaryVolume(data.get(), data.get() + volSize, empty);
    


	return 0;
}
