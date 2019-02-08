//
// Created by jim on 2/7/19.
//

#ifndef jsonindexfile_h__
#define jsonindexfile_h__

#include <bd/io/datatypes.h>
#include <bd/io/fileblock.h>
#include <bd/volume/volume.h>

#include <vector>

namespace bd { namespace indexfile { namespace v2 {
    class JsonIndexFile{
    public:
        void
        open(std::string const & fname);

        int
        version();

        std::string const &
        getRawFileName();

        std::string const &
        getRawFilePath();

        std::string const &
        getTFFileName();

        bd::DataType
        getDatType() const;

        std::vector<bd::FileBlock> const&
        getFileBlocks() const;

        bd::Volume const&
        getVolume() const;

    private:
        bd::Volume m_volume;
        std::vector<bd::FileBlock> m_blocks;
        std::string m_fname;
        std::string m_fpath;
        std::string m_tffname;
    };


}}}


#endif //SUBVOL_JSONINDEXFILE_H
