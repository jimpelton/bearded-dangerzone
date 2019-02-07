//
// Created by jim on 4/14/15.
//

#ifndef bdobj_h__
#define bdobj_h__

#include <string>

namespace bd
{
class BDObj
{
private:
  static unsigned int counter;

public:
  BDObj();
  BDObj(const std::string&);
  virtual ~BDObj();


  ////////////////////////////////////////////////////////////////////////////////
  /// \brief Set the human readable name of this BDObj.
  ////////////////////////////////////////////////////////////////////////////////
  void name(const std::string&);

  ////////////////////////////////////////////////////////////////////////////////
  /// \brief Return a string representation of this BDObj. The base class
  ///        to_string() just returns the name.
  ////////////////////////////////////////////////////////////////////////////////
  virtual std::string to_string() const;

private:
  std::string m_name;
};
} // namespace bd

#endif // !bdobj_h__


