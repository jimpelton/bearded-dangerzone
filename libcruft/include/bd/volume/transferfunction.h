//
// Created by Jim Pelton on 9/4/16.
//

#ifndef bd_transferfunction_h
#define bd_transferfunction_h

#include <bd/util/color.h>

#include <string>
#include <memory>
#include <vector>
#include <sstream>

namespace bd
{
class OpacityTransferFunction;

class ColorTransferFunction;

struct ColorKnot;
struct OpacityKnot;


std::ostream &
operator<<(std::ostream &os, bd::OpacityTransferFunction const &otf);


std::ostream &
operator<<(std::ostream &os, bd::ColorTransferFunction const &otf);


std::ostream &
operator<<(std::ostream &os, bd::Color const &c);


std::ostream &
operator<<(std::ostream &os, bd::ColorKnot const &knot);


std::ostream &
operator<<(std::ostream &os, bd::OpacityKnot const &knot);


/// \brief A knot in an opacity transfer function
struct OpacityKnot
{
  double s;
  double alpha;


  bool
  operator==(OpacityKnot const &rhs) const
  {
    return s == rhs.s;
  }


  bool
  operator!=(OpacityKnot const &rhs) const
  {
    return !( *this == rhs );
  }


  bool
  operator<(OpacityKnot const &rhs) const
  {
    return s < rhs.s;
  }


  bool
  operator>(OpacityKnot const &rhs) const
  {
    return rhs < *this;
  }


  bool
  operator<=(OpacityKnot const &rhs) const
  {
    return !( *this > rhs );
  }


  bool
  operator>=(OpacityKnot const &rhs) const
  {
    return !( *this < rhs );
  }


  std::string
  to_string() const
  {
    std::stringstream ss;
    ss << "{S: " << s << ", A: " << alpha << '}';
    return ss.str();
  }


}; // struct OpacityKnot


struct ColorKnot
{
  double s;
  Color c;


  bool
  operator==(ColorKnot const &rhs) const
  {
    return s == rhs.s;
  }


  bool
  operator!=(ColorKnot const &rhs) const
  {
    return !( *this == rhs );
  }


  bool
  operator<(ColorKnot const &rhs) const
  {
    return s < rhs.s;
  }


  bool
  operator>(ColorKnot const &rhs) const
  {
    return rhs < *this;
  }


  bool
  operator<=(ColorKnot const &rhs) const
  {
    return !( *this > rhs );
  }


  bool
  operator>=(ColorKnot const &rhs) const
  {
    return !( *this < rhs );
  }


  std::string
  to_string() const
  {
    std::stringstream ss;
    ss << "{S: " << s << ", C: " << c << '}';
    return ss.str();
  }

}; // struct ColorKnot


template<class Knot, class Value>
class TransferFunction
{
public:
  TransferFunction()
  {
  }


  virtual ~TransferFunction()
  {
  }
//  TransferFunction(TransferFunction const &) = default;
//  TransferFunction(TransferFunction &&) = default;

  /// \brief Parses a 1d scalar transfer function in a text file
  /// into a vector of OpacityKnot.
  ///
  /// \note The file can use double precision floating pt values.
  ///
  /// \param filename The path to the text file that has the scalar transfer function.
  /// \throws std::runtime_error If the file could not be parsed.
  /// \throws std::ifstream::failure If there was a problem reading the file.
  virtual int
  load(std::string const &filename) = 0;


  virtual Value
  interpolate(double scalar) const = 0;


  size_t
  getNumKnots()
  {
    return _knots.size();
  }


  std::vector<Knot> const &
  getKnotsVector() const
  {
    return _knots;
  }


  virtual std::string
  to_string() const
  {
    std::stringstream ss;
    ss << '{';
    for (auto &k : _knots) {
      ss << k << ", ";
    }
    ss << '}';

    return ss.str();
  }


protected:
  std::vector<Knot> _knots;

};

/// \brief Represents an opacity transfer function
///
/// The file should have the number of knots on the first line
/// with the following lines each containing one knot. The knots
/// are pairs of double precision floating point values. The
/// first value is the knot scalar value (normalized!) and
/// the second value is the knot alpha value.
class OpacityTransferFunction
    : public TransferFunction<OpacityKnot, double>
{
public:

  OpacityTransferFunction();
  virtual ~OpacityTransferFunction();


  int
  load(std::string const &filename) override;


  /// \throws std::runtime_error if v is not in [0.0..1.0]
  double
  interpolate(double v) const override;

}; // class OpacityTransferFunction


/// \brief Represents an color transfer function (color map)
///
/// The file should have the number of knots on the first line
/// with the following lines each containing one knot. The knots
/// are quad tuples of double precision floating point values.
/// Each value is separated by a space. The
/// first value is the knot scalar value (normalized!) and
/// the following three values are the knot red, gree, blue values.
/// The r, g, b values range from 0.0 to 1.0.
class ColorTransferFunction
    : public TransferFunction<ColorKnot, Color>
{
public:
  ColorTransferFunction();
  virtual ~ColorTransferFunction();
//  ColorTransferFunction(ColorTransferFunction const &o) = default;
//  ColorTransferFunction(ColorTransferFunction &&o) = default;

  int
  load(std::string const &filename) override;


  /// \throws std::runtime_error if v is not in [0.0..1.0]
  Color
  interpolate(double scalar) const override;


};  // class ColorTransferFunction

} // namespace bd

#endif // ! bd_transferfunction_h
