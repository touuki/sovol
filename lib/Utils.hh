#ifndef _SOVOL_UTILS_HH
#define _SOVOL_UTILS_HH 1

#include <ctime>
#include <random>

#include "Field.hh"

class Utils {
 private:
  static std::uniform_real_distribution<double> rand;

 public:
  static thread_local std::mt19937 e;
  template <typename Integer,
            typename = std::enable_if_t<std::is_integral_v<Integer>>>
  static Integer factorial(Integer n) {
    if (n < 0) {
      std::cerr << "ERROR:factorial n less than zero." << std::endl;
    }
    Integer v = 1;
    for (Integer i = n; i > 0; i--) {
      v *= i;
    }
    return v;
  };
  static double random(double min = 0., double max = 1.) {
    return rand(e) * (max - min) + min;
  };
  static Vector3<double> randomOnSphere();
  static Vector3<double> velocity(const Vector3<double> &momentum, double mass);
  static double gamma(const Vector3<double> &momentum, double mass);
  static Vector3<double> momentum(const Vector3<double> &velocity, double mass);
  static Vector3<double> lorentzForce(double charge,
                                      const Vector3<double> &velocity,
                                      const EMField<double> &emField);
  static double kineticEnergy(const Vector3<double> &momentum, double mass);
  static double generLaguePoly(int alpha, int k, double value);
};

std::uniform_real_distribution<double> Utils::rand;
thread_local std::mt19937 Utils::e(time(NULL));

// See http://mathworld.wolfram.com/SpherePointPicking.html
inline Vector3<double> Utils::randomOnSphere() {
  static std::uniform_real_distribution<double> rand_theta(0., 2 * M_PI);
  static std::uniform_real_distribution<double> rand_u(-1., 1.);
  double theta = rand_theta(e);
  double u = rand_u(e);
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
                                           const EMField<double> &em) {
  return charge * (em.E + velocity.cross(em.B));
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