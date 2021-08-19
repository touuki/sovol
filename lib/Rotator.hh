#ifndef _SOVOL_ROTATOR_HH
#define _SOVOL_ROTATOR_HH 1

#include "Vector3.hh"

class Rotator {
 public:
  virtual ~Rotator(){};
  virtual Vector3<double> operator()(const Vector3<double> &v) const = 0;
  virtual Vector3<double> reverse(const Vector3<double> &v) const = 0;
};

DEFINE_FACTORY(Rotator)

// See https://en.wikipedia.org/wiki/Euler_angles
//     https://en.wikipedia.org/wiki/Rotation_matrix
class IntrinsicRotator : public Rotator {
 private:
  Vector3<double> rotateX, rotateY, rotateZ;
  Vector3<double> reverseX, reverseY, reverseZ;

 public:
  IntrinsicRotator(double alpha, double beta, double gamma) {
    double m11 = std::cos(alpha) * std::cos(beta);
    double m12 = std::cos(alpha) * std::sin(beta) * std::sin(gamma) -
            std::sin(alpha) * std::cos(gamma);
    double m13 = std::cos(alpha) * std::sin(beta) * std::cos(gamma) +
            std::sin(alpha) * std::sin(gamma);
    double m21 = std::sin(alpha) * std::cos(beta);
    double m22 = std::sin(alpha) * std::sin(beta) * std::sin(gamma) +
            std::cos(alpha) * std::cos(gamma);
    double m23 = std::sin(alpha) * std::sin(beta) * std::cos(gamma) -
            std::cos(alpha) * std::sin(gamma);
    double m31 = -std::sin(beta);
    double m32 = std::cos(beta) * std::sin(gamma);
    double m33 = std::cos(beta) * std::cos(gamma);
    rotateX = Vector3(m11, m12, m13);
    rotateY = Vector3(m21, m22, m23);
    rotateZ = Vector3(m31, m32, m33);
    reverseX = Vector3(m11, m21, m31);
    reverseY = Vector3(m12, m22, m32);
    reverseZ = Vector3(m13, m23, m33);
  };
  Vector3<double> operator()(const Vector3<double> &v) const override {
    return Vector3(rotateX.dot(v), rotateY.dot(v), rotateZ.dot(v));
  };
  Vector3<double> reverse(const Vector3<double> &v) const override {
    return Vector3(reverseX.dot(v), reverseY.dot(v), reverseZ.dot(v));
  };
};

REGISTER_MULTITON(Rotator, IntrinsicRotator)

class ExtrinsicRotator : public Rotator {
 private:
  Vector3<double> rotateX, rotateY, rotateZ;
  Vector3<double> reverseX, reverseY, reverseZ;

 public:
  ExtrinsicRotator(double alpha, double beta, double gamma) {
    double m11 = std::cos(gamma) * std::cos(beta);
    double m12 = std::cos(gamma) * std::sin(beta) * std::sin(alpha) -
            std::sin(gamma) * std::cos(alpha);
    double m13 = std::cos(gamma) * std::sin(beta) * std::cos(alpha) +
            std::sin(gamma) * std::sin(alpha);
    double m21 = std::sin(gamma) * std::cos(beta);
    double m22 = std::sin(gamma) * std::sin(beta) * std::sin(alpha) +
            std::cos(gamma) * std::cos(alpha);
    double m23 = std::sin(gamma) * std::sin(beta) * std::cos(alpha) -
            std::cos(gamma) * std::sin(alpha);
    double m31 = -std::sin(beta);
    double m32 = std::cos(beta) * std::sin(alpha);
    double m33 = std::cos(beta) * std::cos(alpha);
    rotateX = Vector3(m11, m12, m13);
    rotateY = Vector3(m21, m22, m23);
    rotateZ = Vector3(m31, m32, m33);
    reverseX = Vector3(m11, m21, m31);
    reverseY = Vector3(m12, m22, m32);
    reverseZ = Vector3(m13, m23, m33);
  };
  Vector3<double> operator()(const Vector3<double> &v) const override {
    return Vector3(rotateX.dot(v), rotateY.dot(v), rotateZ.dot(v));
  };
  Vector3<double> reverse(const Vector3<double> &v) const override {
    return Vector3(reverseX.dot(v), reverseY.dot(v), reverseZ.dot(v));
  };
};

REGISTER_MULTITON(Rotator, ExtrinsicRotator)

#endif