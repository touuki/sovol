#include "ParticleFactory.hh"
#include "Config.hh"
#include <ctime>

ParticleFactory::ParticleFactory()
    : ParticleFactory(
          Config::getString(SOVOL_CONFIG_KEY(PARTICLES_TYPE),
                            "LeapfrogParticle"),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_MASS), 1.),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_CHARGE), -1.),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_RADIUS)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_LENGTH)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_KINETIC_ENERGY)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_ENERGY_SPREAD)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_ANGULAR_DIVERGENCE)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_POLAR_ANGLE)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_AZIMUTHAL_ANGLE))){};

ParticleFactory::ParticleFactory(const std::string &_className, double _mass,
                                 double _charge, double _radius, double _length,
                                 double _kinetic_energy, double _energy_spread,
                                 double _angular_divergence,
                                 double _polar_angle, double _azimuthal_angle)
    : className(_className), mass(_mass), charge(_charge), radius(_radius),
      length(_length), kinetic_energy(_kinetic_energy),
      energy_spread(_energy_spread), angular_divergence(_angular_divergence),
      polar_angle(_polar_angle), azimuthal_angle(_azimuthal_angle),
      random_engine(std::default_random_engine(time(NULL))),
      kinetic_energy_dist(std::normal_distribution(
          _kinetic_energy, 0.5 * M_SQRT1_2 * _energy_spread * _kinetic_energy)),
      position_r_dist(std::normal_distribution(0., _radius * M_SQRT1_2)),
      position_z_dist(std::uniform_real_distribution(-_length, 0.)),
      position_phi_dist(std::uniform_real_distribution(0., 2 * M_PI)),
      momentum_theta_dist(
          std::normal_distribution(0., 0.5 * M_SQRT1_2 * _angular_divergence)),
      momentum_phi_dist(std::uniform_real_distribution(0., 2 * M_PI)){};

Particle *ParticleFactory::createObject() {
    ParticleConstructor constructor = nullptr;

    if (constructors().find(className) != constructors().end())
        constructor = constructors().find(className)->second;

    if (constructor == nullptr) {
        std::cerr << "ERROR: Create Particle failed. Class " << className
                  << " is not found." << std::endl;
        return nullptr;
    }

    double Ek = kinetic_energy_dist(random_engine);
    while (Ek < 0.)
        Ek = kinetic_energy_dist(random_engine);

    double p = sqrt(pow(Ek + mass, 2) - pow(mass, 2));
    double phi = momentum_phi_dist(random_engine);
    double theta = momentum_theta_dist(random_engine);
    Vector3<double> momentum(p * sin(theta) * cos(phi),
                             p * sin(theta) * sin(phi), p * cos(theta));
    double position_phi = position_phi_dist(random_engine);
    double r = position_r_dist(random_engine);
    Vector3<double> position(r * cos(position_phi), r * sin(position_phi),
                             position_z_dist(random_engine));

    Particle *particle = (*constructor)(position, momentum, charge, mass);
    particle->rotate(polar_angle, azimuthal_angle);
    return particle;
};