
#include <bd/file/datareader.h>
#include <bd/scene/transformable.h>
#include <block.h>

#include <functional>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
// 1. Create binary volume
// 2. Determine empty/non-empty regions from binary volume.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// \brief An axis-aligned bounding box.
//////////////////////////////////////////////////////////////////////////
class AABB : public bd::Transformable
{
public:
    AABB();
    virtual ~AABB();

private:

};


AABB::AABB(){ }


AABB::~AABB(){ }


class TreeNode
{

};


template<typename ValType>
void createBinaryVolume(std::function<bool(ValType)> &isEmpty)
{
    ValType val{ 0 };
    if (isEmpty())
    {

    }
}


int main(int argc, char *argv[])
{
    


	return 0;
}