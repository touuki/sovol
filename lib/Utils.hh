#ifndef _SOVOL_UTILS_HH
#define _SOVOL_UTILS_HH 1

#include "Field.hh"
#include "Vector3.hh"
#include <random>

class Utils {
  private:
    static std::default_random_engine e;
    static std::uniform_real_distribution<double> dist;

  public:
    template <typename T> static T factorial(T n) {
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
    static Vector3<double> velocity(const Vector3<double> &momentum,
                                    double mass);
    static double gamma(const Vector3<double> &momentum, double mass);
    static Vector3<double> momentum(const Vector3<double> &velocity,
                                    double mass);
    static Vector3<double> lorentzForce(double charge,
                                        const Vector3<double> &velocity,
                                        const EMField &emField);
    static double kineticEnergy(const Vector3<double> &momentum, double mass);
};

#endif