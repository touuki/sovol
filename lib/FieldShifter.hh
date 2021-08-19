#ifndef _SOVOL_FIELDSHIFTER_HH
#define _SOVOL_FIELDSHIFTER_HH 1

#include "Field.hh"
#include "Rotator.hh"

class FieldShifter {
 public:
  virtual Vector3<double> reversePosition(const Vector3<double> &) const = 0;
  virtual EMField<double> shiftEMField(const EMField<double> &) const = 0;
};

DEFINE_FACTORY(FieldShifter)

class FieldTranslator : public FieldShifter {
 private:
  Vector3<double> displacement;

 public:
  FieldTranslator();
  Vector3<double> reversePosition(const Vector3<double> &pos) const override {
    return pos - displacement;
  };
  EMField<double> shiftEMField(const EMField<double> &em) const override { return em; };
};

REGISTER_MULTITON(FieldShifter, FieldTranslator)

class FieldRotator : public FieldShifter {
 private:
  Vector3<double> center;
  std::shared_ptr<Rotator> rotator;

 public:
  FieldRotator();
  Vector3<double> reversePosition(const Vector3<double> &pos) const override {
    Vector3 v = pos - center;
    v = rotator->reverse(v);
    return center + v;
  };
  EMField<double> shiftEMField(const EMField<double> &em) const override {
    return EMField{Vector3((*rotator)(em.E)), Vector3((*rotator)(em.B))};
  };
};

REGISTER_MULTITON(FieldShifter, FieldRotator)

#endif