#ifndef _SOVOL_UTILS_HH
#define _SOVOL_UTILS_HH 1

#include <ctime>
#include <random>
#include <thread>

#include "Table.hh"
#include "Field.hh"

namespace utils {

static std::uniform_real_distribution<double> uniform_dist;
static std::normal_distribution<double> normal_dist;
static thread_local std::mt19937 e(
    time(NULL) + std::hash<std::thread::id>{}(std::this_thread::get_id()));

static std::shared_ptr<Table1d> anomalousMagneticMoment;
static std::shared_ptr<Table1d> photonEmissionRate;
static std::shared_ptr<Table1d> photonEmissionRateSpinCorrection;
static std::shared_ptr<Table3d> photonParameter;
static std::shared_ptr<Table1d> k13;
static std::shared_ptr<Table1d> k23;
static std::shared_ptr<Table1d> intK13;
static std::shared_ptr<Table1d> f1;
static std::shared_ptr<Table1d> f2;
static std::shared_ptr<Table1d> f3;

template <typename Integer,
          typename = std::enable_if_t<std::is_integral_v<Integer>>>
inline Integer factorial(Integer n) {
  Integer v = 1;
  for (Integer i = n; i > 0; i--) {
    v *= i;
  }
  return v;
};
inline double generLaguePoly(int alpha, int k, double value) {
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

inline double random(double min = 0., double max = 1.) {
  return uniform_dist(e) * (max - min) + min;
};
inline double normal_distribution(double mean = 0., double stddev = 1.) {
  return normal_dist(e) * stddev + mean;
};
// See http://mathworld.wolfram.com/SpherePointPicking.html
inline Vector3<double> sphere_uniform_distribution(double length = 1.) {
  double theta = random(0., 2 * M_PI);
  double u = random(-1., 1.);
  return Vector3(std::sqrt(1. - std::pow(u, 2)) * std::cos(theta),
                 std::sqrt(1. - std::pow(u, 2)) * std::sin(theta), u) *
         length;
};
// See: https://academic.oup.com/gji/article/203/2/893/575592
inline Vector3<double> fisher_distribution(double kappa, double length = 1.) {
  double cos_theta = std::log(random(std::exp(kappa) - 2 * std::sinh(kappa),
                                     std::exp(kappa))) /
                     kappa;
  double sin_theta = std::sqrt(1. - std::pow(cos_theta, 2));
  double phi = random(0., 2 * M_PI);
  return Vector3(sin_theta * std::cos(phi), sin_theta * std::sin(phi),
                 cos_theta) *
         length;
};
inline Vector3<double> velocity(const Vector3<double> &momentum, double mass) {
  return momentum / std::sqrt(std::pow(mass, 2) + momentum.square());
};
inline double gamma(const Vector3<double> &momentum, double mass) {
  return std::sqrt(1. + momentum.square() / std::pow(mass, 2));
};
inline Vector3<double> momentum(const Vector3<double> &velocity, double mass) {
  return mass / std::sqrt(1. - velocity.square()) * velocity;
};
inline Vector3<double> lorentzForce(double charge,
                                    const Vector3<double> &velocity,
                                    const EMField<double> &emField) {
  return charge * (emField.E + velocity.cross(emField.B));
};
inline double kineticEnergy(const Vector3<double> &momentum, double mass) {
  return std::sqrt(std::pow(mass, 2) + momentum.square()) - mass;
};

};  // namespace utils
#endif