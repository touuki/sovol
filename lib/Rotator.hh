#ifndef _SOVOL_ROTATOR_HH
#define _SOVOL_ROTATOR_HH 1

#include "Vector3.hh"

template <typename T> class Rotator {
  public:
    virtual ~Rotator(){};
    virtual Vector3<T> operator()(const Vector3<T> v) const = 0;
};

// See https://en.wikipedia.org/wiki/Euler_angles
//     https://en.wikipedia.org/wiki/Rotation_matrix
template <typename T> class IntrinsicRotator : public Rotator<T> {
  private:
    Vector3<T> rotateX, rotateY, rotateZ;

  public:
    IntrinsicRotator(T alpha, T beta, T gamma)
        : rotateX(std::cos(alpha) * std::cos(beta),
                  std::cos(alpha) * std::sin(beta) * std::sin(gamma) -
                      std::sin(alpha) * std::cos(gamma),
                  std::cos(alpha) * std::sin(beta) * std::cos(gamma) +
                      std::sin(alpha) * std::sin(gamma)),
          rotateY(std::sin(alpha) * std::cos(beta),
                  std::sin(alpha) * std::sin(beta) * std::sin(gamma) +
                      std::cos(alpha) * std::cos(gamma),
                  std::sin(alpha) * std::sin(beta) * std::cos(gamma) -
                      std::cos(alpha) * std::sin(gamma)),
          rotateZ(-std::sin(beta), std::cos(beta) * std::sin(gamma),
                  std::cos(beta) * std::cos(gamma)) {}
    ~IntrinsicRotator(){};
    Vector3<T> operator()(const Vector3<T> v) const override {
        return Vector3(rotateX.dot(v), rotateY.dot(v), rotateZ.dot(v));
    };
};

#endif