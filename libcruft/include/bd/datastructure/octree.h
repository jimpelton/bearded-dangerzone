
#ifndef bd_octree_h__
#define bd_octree_h__

#include <cstddef>
#include <cmath>

template<class NDataTy>
class Octree
{

private:
  class OcNode
  {

  public:
    void
    insert(OcNode *m_root, NDataTy const &data, float const pos[3])
    {
      OcNode *nodes = m_root;



    }


  private:



  };


public:
  Octree() = delete;

  Octree(int maxPointsPerNode, int maxDepth)
      : m_maxPointsPerNode{ maxPointsPerNode }
      , m_maxDepth{ maxDepth }
  {
    size_t totalNodes{ static_cast<size_t>(std::pow(8, maxDepth)) };
    m_nodes = new OcNode[ totalNodes ];
  }

  Octree(Octree const &rhs)
    : m_nodes{ rhs.m_nodes }
    , m_maxPointsPerNode{ rhs.m_maxPointsPerNode } { }

  bool operator==(Octree const &other)
  {
    return m_nodes == other.m_nodes;
  }

  ~Octree(){ }

  void
  insert(NDataTy const &data, float const pos[3])
  {
    insertHelper(data, pos, m_nodes);
  }



private:
  void insertHelper(NDataTy const &data, float const pos[3], OcNode *node)
  {

  }

  OcNode *m_nodes;
  int m_maxPointsPerNode;
  int m_maxDepth;
};


#endif  // ! bd_octree_h__
