

#ifndef transferfunction_h__
#define transferfunction_h__

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <vector>

struct Color
{
    float r, g, b, a;
};

class Knot
{
public:
    Knot(float scalar, const glm::vec4 &color)
        : m_scalar(scalar)
        , m_color(color) {}
    ~Knot() {}

public:
    float& scalar() { return m_scalar; }
    glm::vec4& color() { return m_color; }

private:
    float m_scalar;
    glm::vec4 m_color;
};

class TransferFunction
{

public:
    TransferFunction();
    ~TransferFunction();

public:
    /** 
     * \brief Load a 1dt transfer function file into a GL_TEXTURE_1D texture.
     * The file is expected to contain every point of the colormap already (in
     * other words, no linear interp is done between the knots).
     * 
     * Returns the texture ID from opengl, or 0 on failure.
     *
     * The file is expected to have the number of lines in the file as the 
     * first line (expcluding line 1 from the total). Following the line count,
     * are space-separated rgba values. The total number of rgba quads should
     * equal the value found on the first line of the file.
     *
     * \param filename The path to the 1dt file.
     * \return 0 on failure, or the texture ID returned from the GL.
     */
    GLuint loadTransfter_1dtformat(const char *filename);

    /**
     * \brief Load the transfer function in file \c filename.
     *
     * This is different from loadTransfer_1dtformat in that the transfer
     * function is not expected to contain every value yet. The knots in the
     * file are each interpolated between to produce a color map of 4096
     * values.
     */
    GLuint loadTransfer1D(const char *filename);

private:
    bool mapsToZeroIndex(Knot n, size_t end);
    bool mapsToLastIndex(Knot n, size_t end);

    float clamp(float v, float min, float max);

    void createColorMap(float map[], size_t mapLength, const std::vector<Knot> &knots);

    size_t toIndex(int scalar, size_t end);
 
private:
    float m_max;
    float m_min;
};


#endif
