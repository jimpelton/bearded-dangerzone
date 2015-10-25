//
// Created by jim on 10/22/15.
//

#ifndef BZONE_VOLUMERENDERER_H
#define BZONE_VOLUMERENDERER_H

#include <glm/fwd.hpp>


class VolumeRenderer {

public:
  VolumeRenderer();
  virtual ~VolumeRenderer();

  virtual void renderSingleFrame();


private:


};


#endif //BZONE_VOLUMERENDERER_H
