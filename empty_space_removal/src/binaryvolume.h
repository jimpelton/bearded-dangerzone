#ifndef binaryvolume_h__
#define binaryvolume_h__

#include "point3.h"

#include <bd/volume/block.h>
#include <bd/util/util.h>

#include <vector>
#include <functional>

template<typename T>
class Region
{
public:
    Region(T u1, T u2, T v1, T v2, T w1, T w2) 
        : u1(u1), u2(u2), v1(v1), v2(v2), w1(w1), w2(w2)
    { }

    T u1, u2;
    T v1, v2;
    T w1, w2;
};

template<typename T>
class Plane
{
public:
    Plane(Point3<T> ul, Point3<T> lr)
    { }

    Point3<T> ul, lr;
};

///////////////////////////////////////////////////////////////////////////////
template<typename Data>
class SummedVolumeTable
{
public:
    using area_type = long long;


    //////////////////////////////////////////////////////////////////////////
    SummedVolumeTable ( size_t volx, size_t voly, size_t volz )
        : m_volx{ volx }
        , m_voly{ voly }
        , m_volz{ volz }
    {
    }


    //////////////////////////////////////////////////////////////////////////
    virtual
    ~SummedVolumeTable()
    {
    }


//    void
//    createBinaryVolume ( Data *in, std::function<bool(Data)> empty )
//    {
//        auto end = in + (m_volx * m_voly * m_volz);
//
//        m_binvol.resize(end-in);
//        auto out = m_binvol.begin();
//
//        while (in != end) {
//            *out++ = empty(*in) ? 0 : 1;
//            ++in;
//        }
//    } // createBinaryVolume


    //////////////////////////////////////////////////////////////////////////
    void
    resizeSumTable()
    {
        size_t size{ m_volx * m_voly * m_volz };
        m_sumtable.resize(size);
    }


    //////////////////////////////////////////////////////////////////////////
    /// \brief Create the summed volume table.
    ///
    /// The parameter \c empty is a functor that returns 0 or 1 depending on 
    /// wether or not the value passed in is deemed to represent an empty
    /// voxel or non-empty voxel, respectively.
    ///
    /// \param in Pointer to volume data
    /// \param empty Determins is value is emptpy.
    //////////////////////////////////////////////////////////////////////////
    void
    createSvt ( Data *in, std::function<int(Data)> empty )
    {
        resizeSumTable();
        for(auto dz =  0ull; dz<m_volz; ++dz) {
            auto z = static_cast<long long>(dz);
        for(auto dy = 0ull; dy<m_voly; ++dy) {
            auto y = static_cast<long long>(dy);
        for(auto dx = 0ull; dx<m_volx; ++dx) {
            auto x = static_cast<long long>(dx);
            size_t idx{ bd::to1D(dx, dy, dz, m_volx, m_voly) };

            if (x - 1 < 0 || y - 1 < 0 || z - 1 < 0) {
                m_sumtable[idx] = 0;
            } 
            else {
                long long v1{ empty(in[idx]) };
                area_type vvvvvv{ 
                    v1 + get(x, y, z - 1)
                    + (get(x - 1, y,     z) - get(x - 1, y, z - 1))
                    + (get(x,     y - 1, z) - get(x, y - 1, z - 1))
                    - (get(x - 1, y - 1, z) - get(x - 1, y - 1, z - 1)) 
                };

                m_sumtable[idx] = vvvvvv;
            }
        }}}
    } // createSvt

    
    //////////////////////////////////////////////////////////////////////////
    /// \brief Create the kd tree after the SVT is created.
    /// First, \c createTree calls \c createSvt first. Then, \c createTree builds a
    /// kd-tree of specified depth.
    ///
    /// \param 
    //////////////////////////////////////////////////////////////////////////
    void 
    createTree(/*std::function<uint64_t(Data)> bv*/)
    {
                
    } // createTree


private:


    //////////////////////////////////////////////////////////////////////////
    area_type
    get ( long long x, long long y, long long z )
    {
        if (x < 0 || y < 0 || z < 0)
            return 0;

        return m_sumtable[bd::to1D(x, y, z, m_volx, m_voly)];

    } // get


    //////////////////////////////////////////////////////////////////////////
    area_type 
    num ( Region<long long> r )
    {
        return (get(r.u2, r.v2, r.w2) - get(r.u2, r.v2, r.w1))
            - (get(r.u1, r.v2, r.w2) - get(r.u1, r.v2, r.w1))
            - (get(r.u2, r.v1, r.w2) - get(r.u2, r.v1, r.w1))
            + (get(r.u1, r.v1, r.w2) - get(r.u1, r.v1, r.w1));
    } // num


    size_t m_volx;  ///< Volume dims X
    size_t m_voly;  ///< Volume dims Y
    size_t m_volz;  ///< Volume dims Z

    std::vector<area_type> m_sumtable; ///< Summed volume table
};

#endif // !binaryvolume_h__

