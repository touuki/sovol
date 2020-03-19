#ifndef _SOVOL_PARTICLEFACTORY_HH
#define _SOVOL_PARTICLEFACTORY_HH 1

#include "Particle.hh"
#include <map>
#include <random>
#include <string>

typedef Particle *(*ParticleConstructor)(const Vector3<double> &,
                                         const Vector3<double> &, double,
                                         double);

class ParticleFactory {
  private:
    const std::string className;
    double mass;
    double charge;
    double radius;
    double length;
    double kinetic_energy;
    double energy_spread;
    double angular_divergence;
    double polar_angle;
    double azimuthal_angle;

    std::default_random_engine random_engine;

    std::normal_distribution<double> kinetic_energy_dist;

    std::normal_distribution<double> position_r_dist;
    std::uniform_real_distribution<double> position_z_dist;
    std::uniform_real_distribution<double> position_phi_dist;

    std::normal_distribution<double> momentum_theta_dist;
    std::uniform_real_distribution<double> momentum_phi_dist;

    inline static std::map<std::string, ParticleConstructor> &constructors() {
        static std::map<std::string, ParticleConstructor> instance;
        return instance;
    }

  public:
    static void registerClass(const std::string &_className,
                              ParticleConstructor constructor) {
        constructors()[_className] = constructor;
    }

    ParticleFactory();
    ParticleFactory(const std::string &_className, double _mass = 1.,
                    double _charge = -1., double _radius = 0.,
                    double _length = 0., double _kinetic_energy = 0.,
                    double _energy_spread = 0., double _angular_divergence = 0.,
                    double _polar_angle = 0., double _azimuthal_angle = 0.);
    Particle *createObject();
};

#define REGISTER_PARTICLE(class_name)                                          \
    class class_name##Helper {                                                 \
      public:                                                                  \
        class_name##Helper() {                                                 \
            ParticleFactory::registerClass(#class_name,                        \
                                           class_name##Helper::creatObjFunc);  \
        }                                                                      \
        static Particle *creatObjFunc(const Vector3<double> &position,         \
                                      const Vector3<double> &momentum,         \
                                      double charge, double mass) {            \
            return new class_name(position, momentum, charge, mass);           \
        }                                                                      \
    };                                                                         \
    class_name##Helper class_name##helper;

#endif