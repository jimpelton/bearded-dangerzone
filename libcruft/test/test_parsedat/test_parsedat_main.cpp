

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <bd/io/datfile.h>
#include <bd/io/datatypes.h>


#define RES_DIR RESOURCE_FOLDER

TEST_CASE("parseDat parses", "[file][parsedat]")
{
    const std::string fileName{ RES_DIR"/walnut.dat" };

    bd::DatFileData dfd;
    bool result = bd::parseDat(fileName, dfd);

    REQUIRE( result == true );
    REQUIRE( dfd.volumeFileName == "walnut.raw" );
    REQUIRE( dfd.dataType == bd::DataType::UnsignedShort);
    REQUIRE( dfd.rX == 400 );
    REQUIRE( dfd.rY == 296 );
    REQUIRE( dfd.rZ == 352 );
}


