#include "mesh.h"

namespace bd {
    namespace geometry {
        Mesh::Mesh(MeshData const & data) : m_data(data)
        { }

        Mesh::~Mesh() { }

        void Mesh::init()
        {
        }
    }
} /* namepsace */