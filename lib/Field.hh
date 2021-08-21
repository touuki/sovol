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
#else
  EMField(Lua &lua) {
    lua.visitField("E");
    E = Vector3(lua);
    lua.leaveTable();
    lua.visitField("B");
    B = Vector3(lua);
    lua.leaveTable();
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