#ifndef _SOVOL_FIELD_HH
#define _SOVOL_FIELD_HH 1

#include "FactoryHelper.hh"
#include "Vector3.hh"

#define C_FUNC_P(func) double (*func)(double, double, double, double)

typedef struct EMField {
  Vector3<double> e;
  Vector3<double> b;
} EMField;

class Field {
 public:
  virtual ~Field();
  virtual EMField operator()(double x, double y, double z,
                             double time) const = 0;
  EMField operator()(const Vector3<double> &position, double time) const;
};

DEFINE_FACTORY(Field)

class CustomField : public Field {
 private:
  C_FUNC_P(ex);
  C_FUNC_P(ey);
  C_FUNC_P(ez);
  C_FUNC_P(bx);
  C_FUNC_P(by);
  C_FUNC_P(bz);
  static double defaultFunction(double, double, double, double);

 public:
  CustomField();
  CustomField(C_FUNC_P(_ex), C_FUNC_P(_ey), C_FUNC_P(_ez), C_FUNC_P(_bx),
              C_FUNC_P(_by), C_FUNC_P(_bz));
  EMField operator()(double x, double y, double z, double time) const override;
};

#endif