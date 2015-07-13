
#include "binaryvolume.h"
#include "cmdline.h"

// bd 
#include <bd/file/datareader.h>
#include <bd/scene/transformable.h>
#include <bd/util/util.h>

#include <bd/volume/block.h>


// std/stl 
#include <functional>
#include <memory>
#include <iostream>
#include <iterator>

//////////////////////////////////////////////////////////////////////////
// 1. Create binary volume.
// 2. Create summed-volume table.
// 3. Built the kd-tree. 
// 4. Generate subvolume list.
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


/////////////////////////////////////////////////////////////////////////
/// \brief Node in the kd-tree, represents an axis-aligned bounding volume          
//////////////////////////////////////////////////////////////////////////
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
    
    int depth;

};



/////////////////////////////////////////////////////////////////////////
/// \brief A kd tree!
//////////////////////////////////////////////////////////////////////////
class KDTree
{
public:
    KDTree();
    virtual ~KDTree();

    TreeNode *root;
};


///////////////////////////////////////////////////////////////////////////////
KDTree::KDTree() 
    : root{ nullptr } { }


///////////////////////////////////////////////////////////////////////////////
KDTree::~KDTree() { }


///////////////////////////////////////////////////////////////////////////////
template<typename ValType>
class IsEmptyFunc
{
public:

    IsEmptyFunc(ValType tmin, ValType tmax)
    : tmin(tmin)
    , tmax(tmax)
    {}


    int
    operator()(ValType v)
    {
        return v > tmin && v < tmax;
    }


private:

    ValType tmin;
    ValType tmax;
};


///////////////////////////////////////////////////////////////////////////////
void usage()
{
    std::cout << "hmm...TODO!" << std::endl;
}


int main(int argc, const char *argv[])
{
    CommandLineOptions opts;
    if (parseThem(argc, argv, opts) == 0) { return 1; }

    std::unique_ptr<float []> data =
        bd::readVolumeData(opts.type, opts.filePath, opts.w, opts.h, opts.d);

    IsEmptyFunc<float> empty(opts.tmin, opts.tmax);
    SummedVolumeTable<float> svt(opts.w, opts.h, opts.d);
    svt.createSvt(data.get(), empty);

    return 0;
}
