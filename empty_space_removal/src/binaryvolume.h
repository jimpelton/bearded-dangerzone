#ifndef binaryvolume_h__
#define binaryvolume_h__

#include <bd/volume/block.h>
#include <bd/util/util.h>

#include <vector>
#include <functional>




///////////////////////////////////////////////////////////////////////////////
template<typename ValType>
class SummedVolumeTable
{
public:
    using area_type = long long;

    SummedVolumeTable ( size_t volx, size_t voly, size_t volz )
        : m_volx{ volx }
        , m_voly{ voly }
        , m_volz{ volz }
    {
    }


    virtual
    ~SummedVolumeTable()
    {
    }


//    void
//    createBinaryVolume ( ValType *in, std::function<bool(ValType)> empty )
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


    void
    resizeSumTable()
    {
        size_t size{ m_volx * m_voly * m_volz };
        m_sumtable.resize(size);
    }


    void
    createSvt ( ValType *in, std::function<int(ValType)> empty )
    {
        resizeSumTable();
        for(auto dz =  0ull ; dz<m_volz; ++dz) {
            auto z = static_cast<long long>(dz);
        for(auto dy = 0ull; dy<m_voly; ++dy) {
            auto y = static_cast<long long>(dy);
        for(auto dx = 0ull; dx<m_volx; ++dx) {
            auto x = static_cast<long long>(dx);
            size_t idx{ bd::to1D(dx, dy, dz, m_volx, m_voly) };

            if (x - 1 < 0 || y - 1 < 0 || z - 1 < 0) {
                m_sumtable[idx] = 0;
            } else {
                long long v1{ empty(in[idx]) };
                area_type dasVal{ v1 + get(x, y, z - 1)
                    + (get(x - 1, y,     z) - get(x - 1, y, z - 1))
                    + (get(x,     y - 1, z) - get(x, y - 1, z - 1))
                    - (get(x - 1, y - 1, z) - get(x - 1, y - 1, z - 1)) };

                m_sumtable[idx] = dasVal;
            }
        }}}
    } // createSvt

    

    
    

private:

    area_type
    get ( long long x, long long y, long long z )
    {
        if (x < 0 || y < 0 || z < 0)
            return 0;

        return m_sumtable[bd::to1D(x, y, z, m_volx, m_voly)];

//        size_t idx { bd::to1D(
//            x > 0 ? x : 0,
//            y > 0 ? y : 0,
//            z > 0 ? z : 0,
//            m_volx,
//            m_voly) };

//        return m_sumtable[idx];
    } // get


    size_t m_volx;
    size_t m_voly;
    size_t m_volz;

    std::vector<area_type> m_sumtable; ///< Summed volume table
};

#endif // !binaryvolume_h__

