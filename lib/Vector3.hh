#ifndef _SOVOL_VECTOR3_HH
#define _SOVOL_VECTOR3_HH 1

#include <cmath>
#include <iostream>

template <typename T>
class Vector3 {
 public:
  T x, y, z;
  static Vector3 zero;
  Vector3();
  Vector3(const T &_x, const T &_y, const T &_z);

  bool operator==(const Vector3 &) const;
  bool operator!=(const Vector3 &) const;
  Vector3 operator-() const;
  Vector3 operator+(const Vector3 &) const;
  Vector3 operator-(const Vector3 &) const;
  Vector3 operator*(const T &) const;
  Vector3 operator/(const T &) const;
  Vector3 &operator=(const Vector3 &);
  Vector3 &operator+=(const Vector3 &);
  Vector3 &operator-=(const Vector3 &);
  Vector3 &operator*=(const T &);
  Vector3 &operator/=(const T &);
  T dot(const Vector3 &) const;
  Vector3 cross(const Vector3 &) const;
  T square() const;
  Vector3 unit() const;
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

template <typename T>
inline Vector3<T>::Vector3() : x(0), y(0), z(0){};

template <typename T>
inline Vector3<T>::Vector3(const T &_x, const T &_y, const T &_z)
    : x(_x), y(_y), z(_z){};

template <typename T>
inline bool Vector3<T>::operator==(const Vector3 &v) const {
  return x == v.x && y == v.y && z == v.z;
};

template <typename T>
inline bool Vector3<T>::operator!=(const Vector3 &v) const {
  return x != v.x || y != v.y || z != v.z;
};

template <typename T>
inline Vector3<T> Vector3<T>::operator-() const {
  return Vector3(-x, -y, -z);
};

template <typename T>
inline Vector3<T> Vector3<T>::operator+(const Vector3 &v) const {
  return Vector3(x + v.x, y + v.y, z + v.z);
};

template <typename T>
inline Vector3<T> Vector3<T>::operator-(const Vector3 &v) const {
  return Vector3(x - v.x, y - v.y, z - v.z);
};

template <typename T>
inline Vector3<T> Vector3<T>::operator*(const T &c) const {
  return Vector3(c * x, c * y, c * z);
};

template <typename T>
inline Vector3<T> Vector3<T>::operator/(const T &c) const {
  return Vector3(x / c, y / c, z / c);
};

template <typename T>
inline Vector3<T> &Vector3<T>::operator=(const Vector3 &v) {
  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
};

template <typename T>
inline Vector3<T> &Vector3<T>::operator+=(const Vector3 &v) {
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
};

template <typename T>
inline Vector3<T> &Vector3<T>::operator-=(const Vector3 &v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
};

template <typename T>
inline Vector3<T> &Vector3<T>::operator*=(const T &c) {
  x *= c;
  y *= c;
  z *= c;
  return *this;
};

template <typename T>
inline Vector3<T> &Vector3<T>::operator/=(const T &c) {
  x /= c;
  y /= c;
  z /= c;
  return *this;
};

template <typename T>
inline T Vector3<T>::dot(const Vector3 &v) const {
  return x * v.x + y * v.y + z * v.z;
};

template <typename T>
inline Vector3<T> Vector3<T>::cross(const Vector3 &v) const {
  return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
};

template <typename T>
inline T Vector3<T>::square() const {
  return x * x + y * y + z * z;
};

template <typename T>
inline Vector3<T> Vector3<T>::unit() const {
  return *this / std::sqrt(square());
};

#endif