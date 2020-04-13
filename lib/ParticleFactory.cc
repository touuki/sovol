#include "ParticleFactory.hh"
#include "Config.hh"
#include "ParticleFactoryProducer.hh"
#include "Vector3.hh"
#include <ctime>

REGISTER_PARTICLEFACTORY(BeamParticleFactory)

BeamParticleFactory::BeamParticleFactory()
    : BeamParticleFactory(
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_CHARGE), -1.),
          Config::getDouble(SOVOL_CONFIG_KEY(PARTICLES_MASS), 1.),
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

BeamParticleFactory::BeamParticleFactory(
    double _charge, double _mass, double _width, double _length,
    double _kinetic_energy, double _energy_spread, double _angular_divergence,
    double _polar_angle, double _azimuthal_angle, double _translation_x,
    double _translation_y, double _translation_z)
    : charge(_charge), mass(abs(_mass)), width(abs(_width)),
      length(abs(_length)), kinetic_energy(abs(_kinetic_energy)),
      energy_spread(abs(_energy_spread)),
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

Vector3<double> BeamParticleFactory::getMomentum() {
    double Ek = kinetic_energy_dist(random_engine);
    while (Ek < 0.)
        Ek = kinetic_energy_dist(random_engine);

    double p = sqrt(pow(Ek + mass, 2) - pow(mass, 2));
    double theta_x = momentum_theta_x_dist(random_engine);
    double theta_y = momentum_theta_y_dist(random_engine);
    double theta = sqrt(pow(theta_x, 2) + pow(theta_y, 2));
    double phi = atan2(theta_y, theta_x);
    return Vector3(p * sin(theta) * cos(phi), p * sin(theta) * sin(phi),
                   p * cos(theta));
};

Vector3<double> BeamParticleFactory::getPosition() {
    return Vector3(position_x_dist(random_engine),
                   position_y_dist(random_engine),
                   position_z_dist(random_engine));
};

void BeamParticleFactory::postCreation(Particle *particle) {
    particle->rotate(polar_angle, azimuthal_angle);
    particle->translate(translation);
}

Particle *BeamParticleFactory::_createParticle() {
    return new Particle(getPosition(), getMomentum(), charge, mass);
};

Particle *BeamParticleFactory::createParticle() {
    Particle *particle = _createParticle();
    postCreation(particle);
    return particle;
};
