#include "FieldShifter.hh"

#include "Rotator.hh"

class FieldTranslator : public FieldShifter {
 private:
  Vector3<double> displacement;

 public:
  FieldTranslator(const Vector3<double> &_displacement)
      : displacement(_displacement){};
#ifdef __EMSCRIPTEN__
  FieldTranslator(emscripten::val v) : displacement(v["displacement"]){};
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
  FieldRotator(emscripten::val v) {
    if (!v["center"].isUndefined()) {
      center = Vector3<double>(v["center"]);
    }
    rotator = RotatorFactory::createObject(v["rotator"]);
  };
#else
  FieldRotator(Lua &lua) {
    int type = lua.visitField("center");
    if (type == LUA_TNIL) {
      lua.popNil();
    } else {
      center = Vector3<double>(lua);
      lua.leaveTable();
    }
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
    return EMField(Vector3(rotator->operator()(em.E)),
                   Vector3(rotator->operator()(em.B)));
  };
};

REGISTER_MULTITON(FieldShifter, FieldRotator)
