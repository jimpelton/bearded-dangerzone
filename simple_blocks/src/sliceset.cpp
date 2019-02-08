//
// Created by Jim Pelton on 12/24/15.
//

#include "sliceset.h"

#include <iostream>


std::ostream &
operator<<(std::ostream &ostr, SliceSet s)
{

  switch (s) {
    case SliceSet::XZ:
      ostr << "XZ";
      return ostr;
    case SliceSet::YZ:
      ostr << "YZ";
      return ostr;
    case SliceSet::XY:
      ostr << "XY";
      return ostr;
    case SliceSet::AllOfEm:
      ostr << "AllOfEm";
      return ostr;
    case SliceSet::NoneOfEm:
      ostr << "NoneOfEm";
      return ostr;
    default:
      ostr << "unknown";
      return ostr;
  }

}

