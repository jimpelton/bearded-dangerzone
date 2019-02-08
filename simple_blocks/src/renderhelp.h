//
// Created by Jim Pelton on 8/1/16.
//

#ifndef SUBVOL_RENDERHELP_H
#define SUBVOL_RENDERHELP_H

#include <GLFW/glfw3.h>

#include "blockcollection.h"
#include "blockrenderer.h"
#include "cmdline.h"

#include <bd/graphics/vertexarrayobject.h>

#include <glm/glm.hpp>
#include <memory>

namespace subvol
{
class Renderer;

class Camera;

class Controls;

class BlockRenderer;

glm::vec3 const g_backgroundColors[2]{
    { 0.15, 0.15, 0.15 },
    { 1.0,  1.0,  1.0 }
};

namespace renderhelp
{
//std::shared_ptr<bd::CoordinateAxis> g_axis{ nullptr }; ///< The coordinate axis lines.
//extern std::shared_ptr<subvol::BlockRenderer> g_renderer;
extern std::shared_ptr<bd::ShaderProgram> g_wireframeShader;
extern std::shared_ptr<bd::ShaderProgram> g_volumeShader;
extern std::shared_ptr<bd::ShaderProgram> g_volumeShaderLighting;
extern std::shared_ptr<bd::VertexArrayObject> g_axisVao;
extern std::shared_ptr<bd::VertexArrayObject> g_boxVao;
extern std::shared_ptr<bd::VertexArrayObject> g_quadVao;
//extern std::shared_ptr<subvol::BlockCollection> g_blockCollection;
//extern std::shared_ptr<bd::IndexFile> g_indexFile;
//std::shared_ptr<subvol::Controls> g_controls{ nullptr };

extern double g_rovMin;
extern double g_rovMax;

//extern BLThreadData g_blThreadData;


GLFWwindow *
initGLContext(int screenWidth, int screenHeight);


BlockLoader *
initializeBlockLoader(bd::IndexFile const *indexFile,
                      subvol::CommandLineOptions const &clo);


BlockCollection *
initializeBlockCollection(BlockLoader *loader,
                          bd::IndexFile const *indexFile,
                          subvol::CommandLineOptions const &clo);


void
setInitialGLState();


void
initializeControls(GLFWwindow *,
                   std::shared_ptr<BlockRenderer>);


void
setRendererInitialTransferFunction(bool loaded,
                                   std::string const &name,
                                   subvol::BlockRenderer &renderer);


bool
initializeTransferFunctions(subvol::CommandLineOptions const &clo);


bool
initializeShaders(subvol::CommandLineOptions const &clo);


void
initializeVertexBuffers(subvol::CommandLineOptions const &clo,
                        bd::Volume const &v,
                        glm::u64vec3 *numSlices);


BlockRenderer *
initializeRenderer(std::shared_ptr<BlockCollection> bc,
                   bd::Volume const &v,
                   subvol::CommandLineOptions const &clo);


/// \brief Get the total and avail memory on the gpu in bytes.
/// If no pointer to avail is provided then just total is returned.
void
queryGPUMemory(int64_t *total,
               int64_t *avail = nullptr);


void
setCameraPosPreset(unsigned int cameraPos);
} // namespace renderhelp
} // namespace subvol


#endif //SUBVOL_GLRENDERHELP_H
