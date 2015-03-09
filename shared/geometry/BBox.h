
#ifndef bbox_h__
#define bbox_h__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace bearded { namespace dangerzone { namespace geometry {

            class BBox {
            public:
                BBox();

                /** \brief create a bbox with the given mins/maxes.
         *
         *  \param minmax minX, maxX, minY, maxY, minZ, maxZ
         */
        BBox
                (
                        const float *minmax);
        ~BBox();

        /* BBox(const BBox &rhs)
             : m_vboId(rhs.m_vboId)
             , m_iboId(rhs.m_iboId)
             , min_x(rhs.min_x)
             , max_x(rhs.max_x)
             , min_y(rhs.min_y)
             , max_y(rhs.max_y)
             , min_z(rhs.min_z)
             , max_z(rhs.max_z)
             , m_transform(rhs.m_transform)
             { }

             BBox& operator=(const BBox &rhs) {

             }*/


    public:
        unsigned vboId() const { return m_vboId; }
        unsigned iboId() const { return m_iboId; }

        glm::mat4 modelTransform() const { return m_transform; }

        void init();

    private:
        static const float vertices[];
        static const unsigned short elements[];

        float min_x, max_x,
            min_y, max_y,
            min_z, max_z;

        unsigned m_vboId, m_iboId;

        glm::mat4 m_transform;
    };
} // namespace geometry
} // namespace dangerzone
} // namspace bearded

#endif
