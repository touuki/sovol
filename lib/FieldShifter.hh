#ifndef _SOVOL_FIELDSHIFTER_HH
#define _SOVOL_FIELDSHIFTER_HH 1

#include "Field.hh"

class FieldShifter {
 public:
  virtual ~FieldShifter(){};
  virtual Vector3<double> reversePosition(const Vector3<double> &) const = 0;
  virtual EMField<double> shiftEMField(const EMField<double> &) const = 0;
};

DEFINE_FACTORY(FieldShifter)

#endif