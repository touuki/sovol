#ifndef _SOVOL_UTILS_HH
#define _SOVOL_UTILS_HH 1

#include <random>

#include "Field.hh"
#include "Vector3.hh"

class Utils {
 private:
  static std::default_random_engine e;
  static std::uniform_real_distribution<double> dist;

 public:
  template <typename T>
  static T factorial(T n) {
    if (n < 0) {
      std::cerr << "ERROR:factorial n less than zero." << std::endl;
    }
    T v = 1;
    for (T i = n; i > 0; i--) {
      v *= i;
    }
    return v;
  };
  static double random(double min = 0., double max = 1.);
  static Vector3<double> randomOnSphere();
  static Vector3<double> velocity(const Vector3<double> &momentum, double mass);
  static double gamma(const Vector3<double> &momentum, double mass);
  static Vector3<double> momentum(const Vector3<double> &velocity, double mass);
  static Vector3<double> lorentzForce(double charge,
                                      const Vector3<double> &velocity,
                                      const EMField &emField);
  static double kineticEnergy(const Vector3<double> &momentum, double mass);
  static double generLaguePoly(int alpha, int k, double value);
};

inline double Utils::random(double min, double max) {
  return min + (max - min) * dist(e);
};

// See http://mathworld.wolfram.com/SpherePointPicking.html
inline Vector3<double> Utils::randomOnSphere() {
  double theta = random(0., 2 * M_PI);
  double u = random(-1., 1.);
  return Vector3(std::sqrt(1. - std::pow(u, 2)) * std::cos(theta),
                 std::sqrt(1. - std::pow(u, 2)) * std::sin(theta), u);
};

inline Vector3<double> Utils::velocity(const Vector3<double> &momentum,
                                       double mass) {
  return momentum / std::sqrt(std::pow(mass, 2) + momentum.square());
};

inline double Utils::gamma(const Vector3<double> &momentum, double mass) {
  return std::sqrt(1. + momentum.square() / std::pow(mass, 2));
};

inline Vector3<double> Utils::momentum(const Vector3<double> &velocity,
                                       double mass) {
  return mass / std::sqrt(1. - velocity.square()) * velocity;
};

inline Vector3<double> Utils::lorentzForce(double charge,
                                           const Vector3<double> &velocity,
                                           const EMField &em) {
  return charge * (em.e + velocity.cross(em.b));
};

inline double Utils::kineticEnergy(const Vector3<double> &momentum,
                                   double mass) {
  return std::sqrt(std::pow(mass, 2) + momentum.square()) - mass;
};

inline double Utils::generLaguePoly(int alpha, int k, double value) {
  if (k == 0) {
    return 1.;
  } else if (k == 1) {
    return 1 + alpha - value;
  } else if (k > 1) {
    return ((2 * k - 1 + alpha - value) * generLaguePoly(alpha, k - 1, value) -
            (k - 1 + alpha) * generLaguePoly(alpha, k - 2, value)) /
           k;
  }
  return 0.;
};

#endif