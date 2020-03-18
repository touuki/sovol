#ifndef _SOVOL_VECTOR3_HH
#define _SOVOL_VECTOR3_HH 1

#include <iostream>

template <typename T> class Vector3 {
  private:
    T x, y, z;

  public:
    Vector3(const T &_x, const T &_y, const T &_z);

    T getX() const;
    T getY() const;
    T getZ() const;
    void setX(const T &);
    void setY(const T &);
    void setZ(const T &);

    /**
     * 这个模板友元函数定义只能写这里，不然链接报错找不到定义。
     * 参考：https://blog.csdn.net/ReaNAiveD/article/details/78736124
     * 或 https://isocpp.org/wiki/faq/templates#template-friends
     *
     */
    template <typename U>
    friend Vector3 operator*(const U &c, const Vector3<T> &v) {
        return Vector3<T>(c * v.x, c * v.y, c * v.z);
    };
    ;
    friend std::ostream &operator<<(std::ostream &output, const Vector3<T> &v) {
        return output << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    };
    bool operator==(const Vector3 &) const;
    bool operator!=(const Vector3 &) const;
    Vector3 operator-() const;
    Vector3 operator+(const Vector3 &) const;
    Vector3 operator-(const Vector3 &) const;
    Vector3 operator*(const T &)const;
    Vector3 operator/(const T &) const;
    Vector3 &operator=(const Vector3 &);
    Vector3 &operator+=(const Vector3 &);
    Vector3 &operator-=(const Vector3 &);
    Vector3 &operator*=(const T &);
    Vector3 &operator/=(const T &);
    T dot(const Vector3 &) const;
    Vector3 cross(const Vector3 &) const;
    T square() const;
};

template <typename T> Vector3<T>::Vector3(const T &_x, const T &_y, const T &_z) {
    x = _x;
    y = _y;
    z = _z;
};

template <typename T> T Vector3<T>::getX() const { return x; };

template <typename T> T Vector3<T>::getY() const { return y; };

template <typename T> T Vector3<T>::getZ() const { return z; };

template <typename T> void Vector3<T>::setX(const T &_x) { x = _x; };

template <typename T> void Vector3<T>::setY(const T &_y) { y = _y; };

template <typename T> void Vector3<T>::setZ(const T &_z) { z = _z; };

template <typename T> bool Vector3<T>::operator==(const Vector3 &v) const {
    return x == v.x && y == v.y && z == v.z;
};

template <typename T> bool Vector3<T>::operator!=(const Vector3 &v) const {
    return x == v.x || y != v.y || z != v.z;
};

template <typename T> Vector3<T> Vector3<T>::operator-() const {
    return Vector3(-x, -y, -z);
};

template <typename T> Vector3<T> Vector3<T>::operator+(const Vector3 &v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
};

template <typename T> Vector3<T> Vector3<T>::operator-(const Vector3 &v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
};

template <typename T> Vector3<T> Vector3<T>::operator*(const T &c) const {
    return Vector3(c * x, c * y, c * z);
};

template <typename T> Vector3<T> Vector3<T>::operator/(const T &c) const {
    T ic = 1 / c;
    return Vector3(ic * x, ic * y, ic * z);
};

template <typename T> Vector3<T> &Vector3<T>::operator=(const Vector3 &v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
};

template <typename T> Vector3<T> &Vector3<T>::operator+=(const Vector3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
};

template <typename T> Vector3<T> &Vector3<T>::operator-=(const Vector3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
};

template <typename T> Vector3<T> &Vector3<T>::operator*=(const T &c) {
    x *= c;
    y *= c;
    z *= c;
    return *this;
};

template <typename T> Vector3<T> &Vector3<T>::operator/=(const T &c) {
    T ic = 1 / c;
    x *= ic;
    y *= ic;
    z *= ic;
    return *this;
};

template <typename T> T Vector3<T>::dot(const Vector3 &v) const {
    return x * v.x + y * v.y + z * v.z;
};

template <typename T> Vector3<T> Vector3<T>::cross(const Vector3 &v) const {
    return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
};

template <typename T> T Vector3<T>::square() const {
    return x * x + y * y + z * z;
};

#endif