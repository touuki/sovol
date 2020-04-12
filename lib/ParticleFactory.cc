#include "ParticleFactory.hh"
#include "Config.hh"
#include "Vector3.hh"
#include <ctime>

ParticleFactory::ParticleFactory()
    : ParticleFactory(
          Config::getString(SOVOL_CONFIG_KEY(PARTICLES_CLASSNAME)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_MASS), 1.),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_CHARGE), -1.),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_WIDTH)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_LENGTH)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_KINETIC_ENERGY)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_ENERGY_SPREAD)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_ANGULAR_DIVERGENCE)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_POLAR_ANGLE)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_AZIMUTHAL_ANGLE)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_TRANSLATION_X)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_TRANSLATION_Y)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_TRANSLATION_Z))){};

ParticleFactory::ParticleFactory(const std::string &_className, double _mass,
                                 double _charge, double _width, double _length,
                                 double _kinetic_energy, double _energy_spread,
                                 double _angular_divergence,
                                 double _polar_angle, double _azimuthal_angle,
                                 double _translation_x, double _translation_y,
                                 double _translation_z)
    : className(_className), mass(abs(_mass)), charge(_charge),
      width(abs(_width)), length(abs(_length)),
      kinetic_energy(abs(_kinetic_energy)), energy_spread(abs(_energy_spread)),
      angular_divergence(abs(_angular_divergence)), polar_angle(_polar_angle),
      azimuthal_angle(_azimuthal_angle),
      translation(Vector3(_translation_x, _translation_y, _translation_z)),
      random_engine(std::default_random_engine(time(NULL))),
      kinetic_energy_dist(std::normal_distribution(
          kinetic_energy,
          kinetic_energy * energy_spread / (100. * 2. * sqrt(2. * M_LN2)))),
      position_x_dist(std::normal_distribution(0., width * 0.25)),
      position_y_dist(std::normal_distribution(0., width * 0.25)),
      position_z_dist(std::uniform_real_distribution(-length, 0.)),
      momentum_theta_x_dist(
          std::normal_distribution(0., angular_divergence * 0.25)),
      momentum_theta_y_dist(
          std::normal_distribution(0., angular_divergence * 0.25)){};

Particle *ParticleFactory::createObject() {
    ParticleConstructor constructor = nullptr;

    if (constructors().find(className) != constructors().end())
        constructor = constructors().find(className)->second;

    if (constructor == nullptr) {
        std::cerr << "ERROR: Create Particle failed. Class " << className
                  << " is not found." << std::endl;
        return nullptr;
    }

    return (*constructor)(this);
};