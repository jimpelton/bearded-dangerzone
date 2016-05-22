//
// Created by Jim Pelton on 5/22/16.
//

#ifndef SUBVOL_COLORMAPS_H
#define SUBVOL_COLORMAPS_H

#include <array>
namespace subvol
{


struct RGBAColor
{
  RGBAColor(float r, float g, float b, float a)
      : r{ r }
      , g{ g }
      , b{ b }
      , a{ a }
  { }


  float r,g,b,a;

};

class ColorMap
{


public:

  const std::array<RGBAColor, 7> FULL_RAINBOW {
      RGBAColor{ 0.00f, 0.93f, 0.51f, 0.93f },
      RGBAColor{ 0.16f, 0.40f, 0.00f, 0.40f },
      RGBAColor{ 0.33f, 0.00f, 0.00f, 1.00f },
      RGBAColor{ 0.50f, 0.00f, 1.00f, 0.00f },
      RGBAColor{ 0.66f, 1.00f, 1.00f, 0.00f },
      RGBAColor{ 0.83f, 1.00f, 0.50f, 0.00f },
      RGBAColor{ 1.00f, 1.00f, 0.00f, 0.00f }
  };

  const std::array<RGBAColor, 7> INVERSE_FULL_RAINBOW {
      RGBAColor{ 0.00f, 1.00f, 0.00f, 0.00f },
      RGBAColor{ 0.16f, 1.00f, 0.50f, 0.00f },
      RGBAColor{ 0.33f, 1.00f, 1.00f, 0.00f },
      RGBAColor{ 0.50f, 0.00f, 1.00f, 0.00f },
      RGBAColor{ 0.66f, 0.00f, 0.00f, 1.00f },
      RGBAColor{ 0.83f, 0.40f, 0.00f, 0.40f },
      RGBAColor{ 1.00f, 0.93f, 0.51f, 0.93f }
  };

/* RAINBOW */
   const std::array<RGBAColor, 5> RAINBOW {
      RGBAColor{ 0.00f, 0.00f, 0.00f, 1.00f },
      RGBAColor{ 0.25f, 0.00f, 1.00f, 1.00f },
      RGBAColor{ 0.50f, 0.00f, 1.00f, 0.00f },
      RGBAColor{ 0.75f, 1.00f, 1.00f, 0.00f },
      RGBAColor{ 1.00f, 1.00f, 0.00f, 0.00f }
  };

   const std::array<RGBAColor, 5> INVERSE_RAINBOW {
      RGBAColor{ 0.00f, 1.00f, 0.00f, 0.00f },
      RGBAColor{ 0.25f, 1.00f, 1.00f, 0.00f },
      RGBAColor{ 0.50f, 0.00f, 1.00f, 0.00f },
      RGBAColor{ 0.75f, 0.00f, 1.00f, 1.00f },
      RGBAColor{ 1.00f, 0.00f, 0.00f, 1.00f }
  };

/* COLD_TO_HOT */
   const std::array<RGBAColor, 3> COLD_TO_HOT {
      RGBAColor{ 0.00f, 0.00f, 0.00f, 1.00f },
      RGBAColor{ 0.50f, 0.75f, 0.00f, 0.75f },
      RGBAColor{ 1.00f, 1.00f, 0.00f, 0.00f }
  };

   const std::array<RGBAColor, 3> HOT_TO_COLD {
      RGBAColor{ 0.00f, 1.00f, 0.00f, 0.00f },
      RGBAColor{ 0.50f, 0.75f, 0.00f, 0.75f },
      RGBAColor{ 1.00f, 0.00f, 0.00f, 1.00f }
  };

/* BLACK_TO_WHITE */
   const  std::array<RGBAColor, 2> BLACK_TO_WHITE {
      RGBAColor{ 0.00f, 0.00f, 0.00f, 0.00f },
      RGBAColor{ 1.00f, 1.00f, 1.00f, 1.00f }
  };

   const std::array<RGBAColor, 2> WHITE_TO_BLACK {
      RGBAColor{ 0.00f, 1.00f, 1.00f, 1.00f },
      RGBAColor{ 1.00f, 0.00f, 0.00f, 0.00f }
  };

/* HSB_HUES */
   const std::array<RGBAColor, 7> HSB_HUES {
      RGBAColor{ 0.00f, 1.00f, 0.00f, 0.00f },
      RGBAColor{ 0.16f, 1.00f, 1.00f, 0.00f },
      RGBAColor{ 0.33f, 0.00f, 1.00f, 0.00f },
      RGBAColor{ 0.50f, 0.00f, 1.00f, 1.00f },
      RGBAColor{ 0.66f, 0.00f, 0.00f, 1.00f },
      RGBAColor{ 0.83f, 1.00f, 0.00f, 1.00f },
      RGBAColor{ 1.00f, 1.00f, 0.00f, 0.00f }
  };

   const std::array<RGBAColor, 7> INVERSE_HSB_HUES {
      RGBAColor{ 0.00f, 1.00f, 0.00f, 0.00f },
      RGBAColor{ 0.16f, 1.00f, 0.00f, 1.00f },
      RGBAColor{ 0.33f, 0.00f, 0.00f, 1.00f },
      RGBAColor{ 0.50f, 0.00f, 1.00f, 1.00f },
      RGBAColor{ 0.66f, 0.00f, 1.00f, 0.00f },
      RGBAColor{ 0.83f, 1.00f, 1.00f, 0.00f },
      RGBAColor{ 1.00f, 1.00f, 0.00f, 0.00f }
  };

/* DAVINCI */
   const std::array<RGBAColor, 11> DAVINCI {
      RGBAColor{ 0.00f, 0.00f, 0.00f, 0.00f },
      RGBAColor{ 0.10f, 0.18f, 0.08f, 0.00f },
      RGBAColor{ 0.20f, 0.27f, 0.18f, 0.08f },
      RGBAColor{ 0.30f, 0.37f, 0.27f, 0.18f },
      RGBAColor{ 0.40f, 0.47f, 0.37f, 0.27f },
      RGBAColor{ 0.50f, 0.57f, 0.47f, 0.37f },
      RGBAColor{ 0.60f, 0.67f, 0.57f, 0.47f },
      RGBAColor{ 0.70f, 0.76f, 0.67f, 0.57f },
      RGBAColor{ 0.80f, 0.86f, 0.76f, 0.67f },
      RGBAColor{ 0.90f, 1.00f, 0.86f, 0.76f },
      RGBAColor{ 1.00f, 1.00f, 1.00f, 1.00f }
  };

   const std::array<RGBAColor, 11> INVERSE_DAVINCI {
      RGBAColor{ 0.00f, 1.00f, 1.00f, 1.00f },
      RGBAColor{ 0.10f, 1.00f, 0.86f, 0.76f },
      RGBAColor{ 0.20f, 0.86f, 0.76f, 0.67f },
      RGBAColor{ 0.30f, 0.76f, 0.67f, 0.57f },
      RGBAColor{ 0.40f, 0.67f, 0.57f, 0.47f },
      RGBAColor{ 0.50f, 0.57f, 0.47f, 0.37f },
      RGBAColor{ 0.60f, 0.47f, 0.37f, 0.27f },
      RGBAColor{ 0.70f, 0.37f, 0.27f, 0.18f },
      RGBAColor{ 0.80f, 0.27f, 0.18f, 0.08f },
      RGBAColor{ 0.90f, 0.18f, 0.08f, 0.00f },
      RGBAColor{ 1.00f, 0.00f, 0.00f, 0.00f }
  };

/* SEISMIC */
   const std::array<RGBAColor, 3> SEISMIC {
      RGBAColor{ 0.00f, 1.00f, 0.00f, 0.00f },
      RGBAColor{ 0.50f, 1.00f, 1.00f, 1.00f },
      RGBAColor{ 1.00f, 0.00f, 0.00f, 1.00f }
  };

   const std::array<RGBAColor, 3> INVERSE_SEISMIC {
      RGBAColor{ 0.00f, 0.00f, 0.00f, 1.00f },
      RGBAColor{ 0.50f, 1.00f, 1.00f, 1.00f },
      RGBAColor{ 1.00f, 1.00f, 0.00f, 0.00f }
  };


}; // class ColorMap

} // namespace subvol

#endif // ! SUBVOL_COLORMAPS_H
