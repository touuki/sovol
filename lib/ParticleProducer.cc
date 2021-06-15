#include "ParticleProducer.hh"
#include "Config.hh"
#include "Vector3.hh"
#include <ctime>

ParticleProducer::~ParticleProducer(){};

REGISTER_MULTITON(ParticleProducer, BeamParticleProducer)

BeamParticleProducer::BeamParticleProducer()
    : BeamParticleProducer(
          Config::getString(SOVOL_CONFIG_KEY(PARTICLE_CLASSNAME), "Particle"),
          Vector3(Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_TRANSLATION_X)),
                  Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_TRANSLATION_Y)),
                  Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_TRANSLATION_Z))),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_CHARGE), -1.),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_MASS), 1.),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_WIDTH)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_LENGTH)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_KINETIC_ENERGY)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_ENERGY_SPREAD)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_ANGULAR_DIVERGENCE)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_POLAR_ANGLE)),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_AZIMUTHAL_ANGLE))){};

BeamParticleProducer::BeamParticleProducer(
    const std::string &_className, const Vector3<double> &_translation,
    double _charge, double _mass, double _width, double _length,
    double _kinetic_energy, double _energy_spread, double _angular_divergence,
    double _polar_angle, double _azimuthal_angle)
    : className(_className), charge(_charge), mass(std::fabs(_mass)),
      width(std::fabs(_width)), length(std::fabs(_length)),
      kinetic_energy(std::fabs(_kinetic_energy)), energy_spread(std::fabs(_energy_spread)),
      angular_divergence(std::fabs(_angular_divergence)), translation(_translation), rotator(_azimuthal_angle, _polar_angle, 0),
      random_engine(std::default_random_engine(Config::getInt(SOVOL_CONFIG_KEY(RANDOM_SEED), time(NULL)))),
      kinetic_energy_dist(std::normal_distribution(
          kinetic_energy,
          kinetic_energy * energy_spread / (100. * 2. * std::sqrt(2. * M_LN2)))),
      position_x_dist(std::normal_distribution(0., width * 0.25)),
      position_y_dist(std::normal_distribution(0., width * 0.25)),
      position_z_dist(std::uniform_real_distribution(-length, 0.)),
      momentum_theta_x_dist(
          std::normal_distribution(0., angular_divergence * 0.25)),
      momentum_theta_y_dist(
          std::normal_distribution(0., angular_divergence * 0.25)){};

std::shared_ptr<Particle> BeamParticleProducer::createParticle() {
    std::shared_ptr<Particle> particle = ParticleFactory::createObject(className);
    double Ek = kinetic_energy_dist(random_engine);
    while (Ek < 0.)
        Ek = kinetic_energy_dist(random_engine);

    double p = std::sqrt(std::pow(Ek + mass, 2) - std::pow(mass, 2));
    double theta_x = momentum_theta_x_dist(random_engine);
    double theta_y = momentum_theta_y_dist(random_engine);
    double theta = std::sqrt(std::pow(theta_x, 2) + std::pow(theta_y, 2));
    double phi = std::atan2(theta_y, theta_x);
    particle->momentum = Vector3(p * std::sin(theta) * std::cos(phi),
                                 p * std::sin(theta) * std::sin(phi), p * std::cos(theta));
    particle->position =
        Vector3(position_x_dist(random_engine), position_y_dist(random_engine),
                position_z_dist(random_engine));
    particle->charge = charge;
    particle->mass = mass;
    particle->position = rotator(particle->position);
    particle->momentum = rotator(particle->momentum);
    particle->translate(translation);
    return particle;
};
