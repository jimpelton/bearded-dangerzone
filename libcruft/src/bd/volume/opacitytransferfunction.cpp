//
// Created by Jim Pelton on 9/8/16.
//

#include <bd/volume/transferfunction.h>
#include <bd/log/logger.h>

#include <vector>
#include <memory>
#include <fstream>
#include <cassert>

namespace bd
{

OpacityTransferFunction::OpacityTransferFunction()
  : TransferFunction{ }
{
}

OpacityTransferFunction::~OpacityTransferFunction()
{
  
}


int
OpacityTransferFunction::load(std::string const &filename)
{
  bd::Dbg() << "Reading OTF: " << filename;
//  bool success{ false };
  _knots.clear();

  size_t lineNum{ 0 };
  int numKnots{ 0 };
  try {
    std::ifstream file;
//    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    file.open(filename.c_str(), std::ifstream::in);
    if (!file.is_open()) {
      bd::Err() << "Couldn't open OTF file: " << filename;
      return -1;
    }

    // number of entries/lines in the scalar opacity file.
    file >> numKnots;
    Dbg() << "OTF file says it has " << numKnots << " knots.";
    lineNum++;

    if (numKnots > 8192) {
      bd::Warn() << "OTF has > 8192 lines.";
    }

    // read rest of file consisting of scalar-opacity pairs
    double s, a;
    while (lineNum <= numKnots && file >> s >> a) {
      _knots.push_back(OpacityKnot{ s, a });
      lineNum++;
    }

    if (lineNum < numKnots) {
      bd::Err() << "Not enough knots!";
      return -1;
//      throw std::runtime_error("Not enough transfer function knots");
    }

    file.close();
//    success = true;

  } catch (std::ios_base::failure &e) {
    bd::Err() << "Problem reading OTF file: " << e.what();
  }
  catch(std::system_error &e) {
    Err() << "Malformed OTF file: " << e.what();
  }
  catch(std::runtime_error &e) {
    Err() << "Exception caught in " << __func__ << ": " << e.what();
  }

  return numKnots;
}


double
OpacityTransferFunction::interpolate(double v_norm) const
{
  if (v_norm < 0.0 || v_norm > 1.0) {
    throw std::runtime_error("v_norm out of range.");
  }

  if(_knots.size() == 0) {
    return 0;
  }
//  assert(v <= 1.0);
//  OpacityKnot prev{ _knots[0] };
//
//  if (v_norm == prev.s) {
//    return prev.alpha;
//  }


  if (v_norm <= _knots[0].s) {
    return _knots[0].alpha;
  }

  if (v_norm >= _knots.back().s) {
    return _knots.back().alpha;
  }

  OpacityKnot k0{ }; 
  OpacityKnot k1{ };  

  size_t const max_idx{ _knots.size() - 1 };
  size_t idx{ static_cast<size_t>((v_norm * max_idx) + 0.5) };

  if (idx > max_idx) {
    k0 = _knots[max_idx - 1];
    k1 = _knots[max_idx];
  }
  else if (idx == 0) {
    k0 = _knots[0];
    k1 = _knots[1];
  }
  else {
    k0 = _knots[idx - 1];
    k1 = _knots[idx];
  }

//  if (v_norm == _knots[idx].s) {
//    return _knots[idx].alpha;
//  }
  
  double d = (v_norm - k0.s) / (k1.s - k0.s);
  return k0.alpha * (1.0 - d) + k1.alpha * d;



//  OpacityKnot prev{ _knots[idx] };
//  OpacityKnot next{ _knots[idx+1] };

//  OpacityKnot next{ _knots[1] };
//  size_t i = 1;
//  while (i < _knots.size() - 1) {
//    if (v > next.s) {
//      prev = next;
//      i += 1;
//      next = _knots[i];
//    } else {
//      break;
//    }
//  }

//  if (v == next.s) {
//    return next.alpha;
//  }

//  return prev.alpha * ( 1.0 - v ) + next.alpha * v;

}

std::ostream&
operator<<(std::ostream &os, bd::OpacityTransferFunction const &otf)
{
  return os << otf.to_string();
}

std::ostream&
operator<<(std::ostream &os, bd::OpacityKnot const &otf)
{
  return os << otf.to_string();
}

} // namespace bd
