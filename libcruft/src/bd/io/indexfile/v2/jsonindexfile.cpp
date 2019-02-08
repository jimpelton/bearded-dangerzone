//
// Created by jim on 2/7/19.
//

#include <bd/io/indexfile/v2/jsonindexfile.h>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include <fstream>
#include <string>

using json = nlohmann::json;

namespace bd {
   void from_json(const json &j, FileBlock& b) {
       auto dims = j.at("dims").get<std::vector<double>>();
       b.world_dims[0] = dims[0];
       b.world_dims[1] = dims[1];
       b.world_dims[2] = dims[2];
       auto origin = j.at("origin").get<std::vector<double>>();
       b.world_oigin[0] = origin[0];
       b.world_oigin[1] = origin[1];
       b.world_oigin[2] = origin[2];
       auto voxDims = j.at("vox_dims").get<std::vector<uint64_t>>();
       b.voxel_dims[0] = voxDims[0];
       b.voxel_dims[1] = voxDims[1];
       b.voxel_dims[2] = voxDims[2];
       j.at("index").get_to(b.block_index);
       auto ijk = j.at("ijk").get<std::vector<uint64_t>>();
       b.ijk_index[0] = ijk[0];
       b.ijk_index[1] = ijk[1];
       b.ijk_index[2] = ijk[2];
       j.at("offset").get_to(b.data_offset);
       j.at("data_bytes").get_to(b.data_bytes);
       j.at("rel").get_to(b.rov);
   }
}

namespace bd { namespace indexfile { namespace v2 {

    namespace {
        glm::u64vec3
        toU64Vec3(json const& js, std::string const&key) {
            auto jsvec = js.at(key).get<std::vector<uint64_t>>();
            glm::u64vec3 vec{ jsvec[0], jsvec[1], jsvec[2], };
            return vec;
        }
    }

    void
    JsonIndexFile::open(std::string const & fname) {
        std::fstream f;
        f.open(fname, std::fstream::in);
        if (! f.is_open()) {

        }
        json js;
        f >> js;
        f.close();

        auto jsVol = js.at("volume");
        auto jsStats = js.at("vol_stats");

        m_tffname = js.at("tr_func").get<std::string>();
        m_fname = jsVol.at("name").get<std::string>();
        m_fpath = jsVol.at("path").get<std::string>();

        Volume v;
        v.avg(jsStats.at("avg").get<double>());
        v.min(jsStats.at("min").get<double>());
        v.max(jsStats.at("max").get<double>());
        v.total(jsStats.at("tot").get<double>());
        v.block_count(toU64Vec3(js, "num_blocks"));
        v.voxelDims(toU64Vec3(jsVol, "vox_dims"));
        v.worldDims(toU64Vec3(jsVol, "world_dims"));

        auto blocks = js.at("blocks").get<std::vector<bd::FileBlock>>();

        m_volume = v;
        m_blocks = blocks;
    }

    int
    JsonIndexFile::version() {
        return 0;
    }

    std::string const &
    JsonIndexFile::getRawFileName() {
        return m_fname;
    }

    std::string const &
    JsonIndexFile::getRawFilePath() {
        return m_fpath;
    }

    std::string const &
    JsonIndexFile::getTFFileName() {
        return m_tffname;
    }

    bd::DataType
    JsonIndexFile::getDatType() const {
        return bd::DataType::UnsignedCharacter;
    }

    std::vector<bd::FileBlock> const&
    JsonIndexFile::getFileBlocks() const {
        return m_blocks;
    }

    bd::Volume const &
    JsonIndexFile::getVolume() const {
        return m_volume;
    }

}}}