#ifndef _SOVOL_FIELDSHIFTER_HH
#define _SOVOL_FIELDSHIFTER_HH 1

#include "Field.hh"
#include "Rotator.hh"

class FieldShifter {
 public:
  virtual ~FieldShifter(){};
  virtual Vector3<double> reversePosition(const Vector3<double> &) const = 0;
  virtual EMField<double> shiftEMField(const EMField<double> &) const = 0;
};

DEFINE_FACTORY(FieldShifter)

class FieldTranslator : public FieldShifter {
 private:
  Vector3<double> displacement;

 public:
  FieldTranslator(const Vector3<double> &_displacement)
      : displacement(_displacement){};
#ifdef __EMSCRIPTEN__
  FieldTranslator(val v) : displacement(v["displacement"]){};
#else
  FieldTranslator(Lua &lua) {
    lua.visitField("displacement");
    displacement = Vector3<double>(lua);
    lua.leaveTable();
  };
#endif
  Vector3<double> reversePosition(const Vector3<double> &pos) const override {
    return pos - displacement;
  };
  EMField<double> shiftEMField(const EMField<double> &em) const override {
    return em;
  };
};

REGISTER_MULTITON(FieldShifter, FieldTranslator)

class FieldRotator : public FieldShifter {
 private:
  Vector3<double> center;
  std::shared_ptr<Rotator> rotator;

 public:
  FieldRotator(const Vector3<double> &_center,
               const std::shared_ptr<Rotator> &_rotator)
      : center(_center), rotator(_rotator){};
#ifdef __EMSCRIPTEN__
  FieldRotator(val v)
      : center(v["center"]),
        rotator(RotatorFactory::createObject(v["rotator"])){};
#else
  FieldRotator(Lua &lua) {
    lua.visitField("center");
    if (!lua.isNil()) {
      center = Vector3<double>(lua);
    }
    lua.leaveTable();
    lua.visitField("rotator");
    rotator = RotatorFactory::createObject(lua);
    lua.leaveTable();
  };
#endif
  Vector3<double> reversePosition(const Vector3<double> &pos) const override {
    Vector3 v = pos - center;
    v = rotator->reverse(v);
    return center + v;
  };
  EMField<double> shiftEMField(const EMField<double> &em) const override {
    return EMField(Vector3((*rotator)(em.E)), Vector3((*rotator)(em.B)));
  };
};

REGISTER_MULTITON(FieldShifter, FieldRotator)

#endif