//
// Created by jim on 4/14/15.
//

#include <bd/util/bdobj.h>

namespace bd
{
unsigned int BDObj::counter = 0;

BDObj::BDObj()
  : BDObj("bd_" + std::to_string(counter))
{
  counter++;
}

BDObj::BDObj(const std::string& name)
  : m_name{ std::string("{" + name + "}") }
{
}

BDObj::~BDObj()
{
}

void
BDObj::name(const std::string& name)
{
  m_name = name;
}

std::string
BDObj::to_string() const
{
  return m_name;
}
} // namespace bd


