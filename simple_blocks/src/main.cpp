
// local includes
#include "cmdline.h"
#include "axis_enum.h"
#include "timing.h"
#include "constants.h"
#include "colormap.h"
#include "renderhelp.h"
#include "controlpanel.h"
#include "blockcollection.h"
#include "semathing.h"
#include "loop.h"
#include "messages/messagebroker.h"

// BD lib
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/log/logger.h>
#include <bd/io/indexfile/indexfile.h>

#include <QApplication>

// STL and STD lib
#include <string>
#include <iostream>
#include <fstream>
#include <future>
#include <memory>


void
cleanup();


void
printBlocks(subvol::BlockCollection *bcol);
//void printNvPmApiCounters(const char *perfOutPath);


using subvol::ColorMapManager;
using subvol::CommandLineOptions;

namespace subvol
{

/////////////////////////////////////////////////////////////////////////////////
void
cleanup()
{
  glfwTerminate();
}


/////////////////////////////////////////////////////////////////////////////////
void
printBlocks(subvol::BlockCollection *bcol)
{
  std::ofstream block_file("blocks.txt", std::ofstream::trunc);

  if (block_file.is_open()) {
    bd::Info() << "Writing blocks to blocks.txt in the current working directory.";
    for (auto &b : bcol->getBlocks()) {
      block_file << b << "\n";
    }
    block_file.flush();
    block_file.close();
  } else {
    bd::Err() << "Could not print blocks because blocks.txt couldn't be created "
                 "in the current working directory.";
  }
}


/////////////////////////////////////////////////////////////////////////////////
//std::shared_ptr<bd::IndexFile>
//openIndexFile(subvol::CommandLineOptions const &clo)
//{
//  // Read the index file to get values from it that we need to populate
//  // the CommandLineOptions struct.
//  std::shared_ptr<bd::IndexFile> indexFile{
//      bd::IndexFile::fromBinaryIndexFile(clo.indexFilePath) };
//  if (indexFile == nullptr) {
//    bd::Err() << "Could open the index file.";
//  }
//
//  return indexFile;
//}


/////////////////////////////////////////////////////////////////////////////////
// Since the IndexFileHeader contains most of the options needed to
// render the volume, we copy those over into the CommandLineOptions struct.
// Without an index file these options are provided via argv anyway.
void
updateCommandLineOptionsFromIndexFile(subvol::CommandLineOptions &clo,
                                      std::shared_ptr<bd::IndexFile> const &indexFile)
{
  bd::Dbg() << "Updating command line options from index file.";
  auto minmaxE =
      std::minmax_element(indexFile->getFileBlocks().begin(),
                          indexFile->getFileBlocks().end(),
                          [](bd::FileBlock const &lhs, bd::FileBlock const &rhs)
                              -> bool {
                            return lhs.rov<rhs.rov;
                          });

  renderhelp::g_rovMin = ( *minmaxE.first ).rov;
  renderhelp::g_rovMax = ( *minmaxE.second ).rov;

  clo.vol_w = indexFile->getVolume().voxelDims().x;
  clo.vol_h = indexFile->getVolume().voxelDims().y;
  clo.vol_d = indexFile->getVolume().voxelDims().z;
  clo.numblk_x = indexFile->getVolume().block_count().x;
  clo.numblk_y = indexFile->getVolume().block_count().y;
  clo.numblk_z = indexFile->getVolume().block_count().z;
  clo.dataType = bd::to_string(bd::IndexFileHeader::getType(indexFile->getHeader()));
}


/////////////////////////////////////////////////////////////////////////////////
GLFWwindow *
init_gl(subvol::CommandLineOptions &clo)
{

  subvol::printThem(clo);

  // Initialize OpenGL and GLFW and generate our transfer function textures.
  GLFWwindow *window{ nullptr };
  window = subvol::renderhelp::initGLContext(clo.windowWidth, clo.windowHeight);
  if (window==nullptr) {
    bd::Err() << "Could not initialize GLFW, exiting.";
    return nullptr;
  }
  bd::Info() << "Open GL initialized.";

  int64_t totalMemory{ 0 };
  subvol::renderhelp::queryGPUMemory(&totalMemory);
  bd::Info() << "GPU memory: " << ( totalMemory*1e-6 ) << "MB";

  if (clo.gpuMemoryBytes>totalMemory) {
    bd::Warn() << "Requested m_gpu memory, " << clo.gpuMemoryBytes
               << " greater than actual GPU memory, using " << totalMemory << " bytes.";
    clo.gpuMemoryBytes = static_cast<size_t>(totalMemory);
  }

  return window;

} // init_gl

} // namespace subvol


/////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char *argv[])
{
  subvol::Broker::start();

  TCLAP::CmdLine cmd("Simple Blocks blocking volume rendering experiment.", ' ');
  subvol::CommandLineOptions clo;
  if (subvol::parseThem(argc, argv, cmd, clo)==0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info."
              << std::endl;
    //    return 1;
  }

  // Open the index file if possible, then setup the BlockCollection
  // and give away ownership of the index file to the BlockCollection.
  std::shared_ptr<bd::IndexFile> indexFile{ std::make_shared<bd::IndexFile>() };
  if (!clo.indexFilePath.empty()) {
    bool ok = false;
    indexFile = std::move(
        bd::IndexFile::fromBinaryIndexFile(clo.indexFilePath, ok));
    if (!ok) {
      bd::Err() << "Could not read index file " << clo.indexFilePath;
      return 1;
    }
    // there are some CL opts that can be specified in the index file, so we 
    // read those into our CommandLineOptions struct.
    updateCommandLineOptionsFromIndexFile(clo, indexFile);
  }

  bd::Info() << "Initializing subvol...";
  GLFWwindow *window{ subvol::init_gl(clo) };
  if (window==nullptr) {
    bd::Err() << "Could not initialize subvol. Exiting...";
    return 1;
  }

  subvol::BlockLoader *loader{
      subvol::renderhelp::initializeBlockLoader(indexFile.get(), clo) };

  std::shared_ptr<subvol::BlockCollection> bc{
      subvol::renderhelp::initializeBlockCollection(loader, indexFile.get(), clo) };

//  std::shared_ptr<subvol::BlockRenderer> br{
//    subvol::renderhelp::initializeRenderer(bc, indexFile->getVolume().worldDims(), clo) };

  std::shared_ptr<subvol::BlockRenderer> br{
      subvol::renderhelp::initializeRenderer(bc, indexFile->getVolume(), clo) };

  subvol::renderhelp::initializeControls(window, br);
//  subvol::renderhelp::BenchmarkLoop loop(window, br, bc, glm::vec3{ 1,0,0 });
  subvol::renderhelp::Loop loop(window, br, bc);

  subvol::Semathing s(1);

  // Start the qt event stuff on a separate thread (this gui is totally kludged in here...).
  std::future<int> returned =
      std::async(std::launch::async,
                 [&]() {
                   QApplication a(argc, argv);
                   subvol::ControlPanel panel(indexFile->getVolume(),
                                              loader->maxGpuBlocks(),
                                              loader->maxMainBlocks());

                   panel.setGlobalRovMinMax(subvol::renderhelp::g_rovMin,
                                            subvol::renderhelp::g_rovMax);
                   panel.setcurrentMinMaxSliders(0, 0);
                   panel.show();
                   s.signal();
                   return a.exec();
                 });

  s.wait();
  loop.loop();
  std::cout << "Waiting for GUI to close..." << std::endl;
  returned.wait();
  std::cout << "subvol exiting: " << returned.get() << std::endl;
//  subvol::timing::printTimes(std::cout);
  //  printNvPmApiCounters(clo.perfOutPath.c_str());
  subvol::cleanup();

  return 0;
}
