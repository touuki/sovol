#ifndef _SOVOL_VECTOR3_HH
#define _SOVOL_VECTOR3_HH 1

#include <cmath>
#include <iostream>

#include "config.hh"

template <typename T>
class Vector3 {
 public:
  T x, y, z;
  static Vector3 zero;
  Vector3() : x(0), y(0), z(0){};
  Vector3(const T &_x, const T &_y, const T &_z) : x(_x), y(_y), z(_z){};
#ifdef __EMSCRIPTEN__
  Vector3(val v) : x(v[0].as<T>()), y(v[1].as<T>()), z(v[2].as<T>()){};
#else
  Vector3(Lua &lua)
      : x(lua.getField<T>(1)), y(lua.getField<T>(2)), z(lua.getField<T>(3)){};
#endif

  bool operator==(const Vector3 &v) const {
    return x == v.x && y == v.y && z == v.z;
  };
  bool operator!=(const Vector3 &v) const {
    return x != v.x || y != v.y || z != v.z;
  };
  Vector3 operator-() const { return Vector3(-x, -y, -z); };
  Vector3 operator+(const Vector3 &v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
  };
  Vector3 operator-(const Vector3 &v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
  };
  Vector3 operator*(const T &c) const { return Vector3(c * x, c * y, c * z); };
  Vector3 operator/(const T &c) const { return Vector3(x / c, y / c, z / c); };
  Vector3 &operator=(const Vector3 &v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
  };
  Vector3 &operator+=(const Vector3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  };
  Vector3 &operator-=(const Vector3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  };
  Vector3 &operator*=(const T &c) {
    x *= c;
    y *= c;
    z *= c;
    return *this;
  };
  Vector3 &operator/=(const T &c) {
    x /= c;
    y /= c;
    z /= c;
    return *this;
  };
  T dot(const Vector3 &v) const { return x * v.x + y * v.y + z * v.z; };
  Vector3 cross(const Vector3 &v) const {
    return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  };
  T square() const { return x * x + y * y + z * z; };
  Vector3 unit() const { return *this / std::sqrt(square()); };
  /**
   * Define template friends here to get rid of linker errors.
   * See https://isocpp.org/wiki/faq/templates#template-friends
   */
  template <typename U>
  friend Vector3 operator*(const U &c, const Vector3<T> &v) {
    return Vector3<T>(c * v.x, c * v.y, c * v.z);
  };
  ;
  friend std::ostream &operator<<(std::ostream &output, const Vector3<T> &v) {
    return output << '(' << v.x << ',' << v.y << ',' << v.z << ')';
  };
};

template <typename T>
Vector3<T> Vector3<T>::zero = Vector3();

#endif