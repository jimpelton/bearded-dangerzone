#include "transferfunction.h"

#include <log/gl_log.h>

#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <fstream>

TransferFunction::TransferFunction()
{
}


TransferFunction::~TransferFunction()
{
}

bool TransferFunction::mapsToZeroIndex(Knot n, size_t numColors) 
{
    return (0 == toIndex(n.scalar(), numColors));
}

bool TransferFunction::mapsToLastIndex(Knot n, size_t numColors) 
{
    return (numColors - 4 == toIndex(n.scalar(), numColors));
}

GLuint TransferFunction::loadTransfter_1dtformat(const char *filename)
{
    std::ifstream file(filename, std::ifstream::in);
    if (!file.is_open()) {
        gl_log_err("caint open tfunc file: %s", filename);
        return 0;
    }

    size_t numTexels;
    file >> numTexels; // number of entries/lines in the 1dt file.
    if (numTexels > 8192) {
        gl_log_err("The 1dt transfer function reports %d knots (max is 8192)." 
                "Skipping loading the transfer function file.", numTexels);
        return 0;
    }

    Color *rgba = new Color[numTexels];

    size_t lineNum = 1;
    float r, g, b, a;
    while (file >> r >> g >> b >> a && lineNum < numTexels) { // every line after line 1 is a space sep'd quad.
        rgba[lineNum] = { r, g, b, a };
        lineNum++;
    }

    file.close();

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_1D, texId);
    glTexImage1D
        (
        GL_TEXTURE_1D,
        0,
        GL_RGBA,
        numTexels,
        0,
        GL_RGBA, GL_FLOAT,
        (void*)rgba
        );

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    //checkForAndLogGlError(__func__, __LINE__);

    delete[] rgba;
    return texId;
}

float TransferFunction::clamp(float val, float min, float max)
{
    if (val > max)
        return max;
    else if (val < min)
        return min;
    else
        return val;
}

size_t TransferFunction::toIndex(int scalar, size_t arrayLength)
{
    //    arrayLength = arrayLength - 1;
    size_t idx = floor((clamp(scalar, m_min, m_max) / m_max) * arrayLength);

    if (idx == arrayLength)
        return idx - 4;

    return idx - (idx % 4);
}

void TransferFunction::createColorMap(float map[], size_t numColors,
    const std::vector<Knot> &knots)
{
    std::vector<Knot>::const_iterator it = knots.cbegin();
    std::vector<Knot>::const_iterator itEnd = knots.cend();

    int idx = 0,
        lastidx = 0,
        distance = 0;
    float dt = 0;
    float *color = nullptr;

    Knot prevKnot = *it;

    for (; it != itEnd; ++it) {
        Knot k = *it;
        idx = toIndex(k.scalar(), numColors);
        distance = idx - lastidx;

        if (distance == 0) { // overwrite the last color

            glm::vec4 c = k.color();
            color = &map[lastidx * 4];
            color[0] = c.r;
            color[1] = c.g;
            color[2] = c.b;
            color[3] = c.a;

        } else { // fill map with colors from previous knot, to this knot.

            dt = 1.0f / float(distance);
            for (int i = 0; i < distance; ++i) {
                float t = i * dt;
                glm::vec4 c = prevKnot.color() + (k.color() - prevKnot.color()) * t;
                color = &map[(lastidx + i) * 4];
                color[0] = c.r;
                color[1] = c.g;
                color[2] = c.b;
                color[3] = c.a;
            }

        } // if 

        lastidx = lastidx + distance;
        prevKnot = k;

    } // for (it!=itEnd)
}

GLuint TransferFunction::loadTransfer1D(const char *filename)
{
    std::ifstream file(filename, std::ifstream::in);
    if (!file.is_open()) {
        gl_log_err("caint open tfunc file: %s", filename);
        return 0;
    }

    const size_t numTexels = 4096;
    float s, r, g, b, a;
    std::vector<Knot> knots;
    while (file >> s >> r >> g >> b >> a &&
        knots.size() < numTexels) {
        glm::vec4 color(r, g, b, a);
        Knot k(s, glm::vec4(r, g, b, a));
        knots.push_back(k);
        gl_log("Tfunc: %f, %s", k.scalar(), glm::to_string(k.color()).c_str());
    }
    file.close();

    if (!mapsToZeroIndex(knots.front(), numTexels)){
        std::cout << "pushed m_min knot to front of transfer function." << std::endl;
        knots.insert(knots.begin(), Knot(m_min, glm::vec4(0)));
    }

    std::cout << toIndex(knots.back().scalar(), numTexels) << std::endl;

    if (!mapsToLastIndex(knots.back(), numTexels)) {
        std::cout << "pushed m_max knot to back of transfer function." << std::endl;
        knots.push_back(Knot(m_max, glm::vec4(0)));
    }

    float *rgbData = new float[numTexels * 4];
    createColorMap(rgbData, numTexels, knots);
    //FILE *f;
    //f = fopen("debug_text.txt", "w");
    //for (size_t i = 0; i < numTexels * 4; i += 4) {
    //    fprintf(f, "%.8f %.8f %.8f %.8f\n", rgbData[i], rgbData[i + 1], rgbData[i + 2], rgbData[i + 3]);
    //}
    //fclose(f);

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_1D, texId);
    glTexImage1D
        (
        GL_TEXTURE_1D,
        0,
        GL_RGBA,
        numTexels,
        0,
        GL_RGBA, GL_FLOAT,
        rgbData
        );

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    //checkForAndLogGlError(__func__, __LINE__);

    delete[] rgbData;
    return texId;
}
