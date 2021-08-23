#ifndef _SOVOL_FIELD_HH
#define _SOVOL_FIELD_HH 1

#include "Vector3.hh"

template <typename T>
class EMField {
 public:
  Vector3<T> E;
  Vector3<T> B;
  EMField(){};
  EMField(const Vector3<T> &_E, const Vector3<T> &_B) : E(_E), B(_B){};
#ifdef __EMSCRIPTEN__
  EMField(emscripten::val v) : E(v["E"]), B(v["B"]){};
  emscripten::val toEmscriptenVal() const {
    emscripten::val v = emscripten::val::object();
    v.set("E", E.toEmscriptenVal());
    v.set("B", B.toEmscriptenVal());
    return v;
  };
#else
  EMField(Lua &lua)
      : E(lua.getField<Vector3<double> >("E")),
        B(lua.getField<Vector3<double> >("B")){};
  void luaPush(Lua &lua) const {
    lua.createTable(0, 2);
    lua.setField("E", E);
    lua.setField("B", B);
  };
#endif
  EMField operator+(const EMField &em) const {
    return EMField(E + em.E, B + em.B);
  };
  EMField &operator+=(const EMField &em) {
    E += em.E;
    B += em.B;
    return *this;
  };
};

class Field {
 public:
  virtual ~Field(){};
  virtual EMField<double> operator()(const Vector3<double> &pos,
                                     double time) const = 0;
};

DEFINE_FACTORY(Field)

#endif