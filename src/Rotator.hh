#ifndef _SOVOL_ROTATOR_HH
#define _SOVOL_ROTATOR_HH 1

#include "Vector3.hh"

class Rotator {
 public:
  virtual ~Rotator(){};
  virtual Vector3<double> operator()(const Vector3<double> &v) const = 0;
  virtual Vector3<double> reverse(const Vector3<double> &v) const = 0;
};

DEFINE_FACTORY(Rotator)

#endif