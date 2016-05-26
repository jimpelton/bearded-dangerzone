//
// Created by Jim Pelton on 5/22/16.
//


#include <map>
#include "colormap.h"


namespace subvol
{

const std::array<glm::vec4, 7> ColorMap::FULL_RAINBOW {
    glm::vec4{ 0.00f, 0.93f, 0.51f, 0.93f },
    glm::vec4{ 0.16f, 0.40f, 0.00f, 0.40f },
    glm::vec4{ 0.33f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.66f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.83f, 1.00f, 0.50f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};

const std::array<glm::vec4, 7> ColorMap::INVERSE_FULL_RAINBOW {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.16f, 1.00f, 0.50f, 0.00f },
    glm::vec4{ 0.33f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.66f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.83f, 0.40f, 0.00f, 0.40f },
    glm::vec4{ 1.00f, 0.93f, 0.51f, 0.93f }
};

const std::array<glm::vec4, 5> ColorMap::RAINBOW {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.25f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.75f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};


const std::array<glm::vec4, 5> ColorMap::INVERSE_RAINBOW {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.25f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.75f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::array<glm::vec4, 3> ColorMap::COLD_TO_HOT {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 0.75f, 0.00f, 0.75f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};


const std::array<glm::vec4, 3> ColorMap::HOT_TO_COLD {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.50f, 0.75f, 0.00f, 0.75f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const  std::array<glm::vec4, 2> ColorMap::BLACK_TO_WHITE {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 1.00f }
};

const std::array<glm::vec4, 2> ColorMap::WHITE_TO_BLACK {
    glm::vec4{ 0.00f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f }
};

const std::array<glm::vec4, 7> ColorMap::HSB_HUES {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.16f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.33f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 0.66f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.83f, 1.00f, 0.00f, 1.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};

const std::array<glm::vec4, 7> ColorMap::INVERSE_HSB_HUES {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.16f, 1.00f, 0.00f, 1.00f },
    glm::vec4{ 0.33f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 0.66f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.83f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};


const std::array<glm::vec4, 11> ColorMap::DAVINCI {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 0.10f, 0.18f, 0.08f, 0.00f },
    glm::vec4{ 0.20f, 0.27f, 0.18f, 0.08f },
    glm::vec4{ 0.30f, 0.37f, 0.27f, 0.18f },
    glm::vec4{ 0.40f, 0.47f, 0.37f, 0.27f },
    glm::vec4{ 0.50f, 0.57f, 0.47f, 0.37f },
    glm::vec4{ 0.60f, 0.67f, 0.57f, 0.47f },
    glm::vec4{ 0.70f, 0.76f, 0.67f, 0.57f },
    glm::vec4{ 0.80f, 0.86f, 0.76f, 0.67f },
    glm::vec4{ 0.90f, 1.00f, 0.86f, 0.76f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 1.00f }
};

const std::array<glm::vec4, 11> ColorMap::INVERSE_DAVINCI {
    glm::vec4{ 0.00f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 0.10f, 1.00f, 0.86f, 0.76f },
    glm::vec4{ 0.20f, 0.86f, 0.76f, 0.67f },
    glm::vec4{ 0.30f, 0.76f, 0.67f, 0.57f },
    glm::vec4{ 0.40f, 0.67f, 0.57f, 0.47f },
    glm::vec4{ 0.50f, 0.57f, 0.47f, 0.37f },
    glm::vec4{ 0.60f, 0.47f, 0.37f, 0.27f },
    glm::vec4{ 0.70f, 0.37f, 0.27f, 0.18f },
    glm::vec4{ 0.80f, 0.27f, 0.18f, 0.08f },
    glm::vec4{ 0.90f, 0.18f, 0.08f, 0.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f }
};

const std::array<glm::vec4, 3> ColorMap::SEISMIC {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.50f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::array<glm::vec4, 3> ColorMap::INVERSE_SEISMIC{
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};

const std::map<std::string, glm::vec4*> ColorMap::maps{
    { "FULL_RAINBOW",         FULL_RAINBOW.data() },
    { "INVERSE_FULL_RAINBOW", INVERSE_FULL_RAINBOW.data() },
    { "RAINBOW",              RAINBOW.data() },
    { "INVERSE_RAINBOW",      INVERSE_RAINBOW.data() },
    { "COLD_TO_HOT",          COLD_TO_HOT.data() },
    { "HOT_TO_COLD",          HOT_TO_COLD.data() },
    { "BLACK_TO_WHITE",       BLACK_TO_WHITE.data() },
    { "WHITE_TO_BLACK",       WHITE_TO_BLACK.data() },
    { "HSB_HUES",             HSB_HUES.data() },
    { "INVERSE_HSB_HUES",     INVERSE_HSB_HUES.data() },
    { "DAVINCI",              DAVINCI.data() },
    { "INVERSE_DAVINCI",      INVERSE_DAVINCI.data() },
    { "SEISMIC",              SEISMIC.data() },
    { "INVERSE_SEISMIC",      INVERSE_SEISMIC.data() }
};

} // namespace subvol