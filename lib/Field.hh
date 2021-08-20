#ifndef _SOVOL_FIELD_HH
#define _SOVOL_FIELD_HH 1

#include "Vector3.hh"

template <typename T>
class EMField {
 public:
  Vector3<T> E;
  Vector3<T> B;
  EMField(const Vector3<T> &_E, const Vector3<T> &_B) : E(_E), B(_B){};
#ifdef __EMSCRIPTEN__
  EMField(val v) : E(v["E"]), B(v["B"]){};
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
};

class Field {
 public:
  virtual ~Field(){};
  virtual EMField<double> operator()(const Vector3<double> &pos,
                                     double time) const = 0;
};

DEFINE_FACTORY(Field)

class CustomField : public Field {
#ifdef __EMSCRIPTEN__
 private:
  val func;

 public:
  CustomField(val v) : func(v["func"]){};
  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    return EMField(func(pos.x, pos.y, pos.z, t));
  };
#else
 private:
  std::string global_function_name;

 public:
  CustomField(Lua &lua)
      : global_function_name(lua.getField("global_function_name")){};
  EMField<double> operator()(const Vector3<double> &pos,
                             double t) const override {
    Lua &lua = Lua::getInstance();
    lua.visitGlobal(global_function_name.c_str());
    lua.call(1, pos.x, pos.y, pos.z, t);
    EMField<double> em(lua);
    lua.leaveTable();
    return em;
  };
#endif
};

REGISTER_MULTITON(Field, CustomField)

#endif