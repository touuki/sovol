#include "Particle.hh"
#include "ParticleFactory.hh"
#include "Utils.hh"

Particle::Particle(const Vector3<double> &_position,
                   const Vector3<double> &_momentum, double _charge,
                   double _mass)
    : position(_position), momentum(_momentum), charge(_charge), mass(_mass){};

Particle::~Particle(){};

// See https://en.wikipedia.org/wiki/Euler_angles
//     https://en.wikipedia.org/wiki/Rotation_matrix
void Particle::rotate(double beta, double alpha) {
    Vector3<double> rotateX(cos(alpha) * cos(beta), -sin(alpha),
                            cos(alpha) * sin(beta));
    Vector3<double> rotateY(sin(alpha) * cos(beta), cos(alpha),
                            sin(alpha) * sin(beta));
    Vector3<double> rotateZ(-sin(beta), 0., cos(beta));
    position = Vector3(rotateX.dot(position), rotateY.dot(position),
                       rotateZ.dot(position));
    momentum = Vector3(rotateX.dot(momentum), rotateY.dot(momentum),
                       rotateZ.dot(momentum));
};

void Particle::translate(const Vector3<double> &translation) {
    position += translation;
}

REGISTER_PARTICLE(RungeKuttaParticle)

RungeKuttaParticle::RungeKuttaParticle(const Vector3<double> &_position,
                                       const Vector3<double> &_momentum,
                                       double _charge, double _mass)
    : Particle(_position, _momentum, _charge, _mass){};

void RungeKuttaParticle::nextStep(const Field &field, double time, double dt) {
    double halfStep = .5 * dt;
    EMField em = field.get(position, time);
    Vector3<double> dxdt1 = Utils::velocity(momentum, mass);
    Vector3<double> dpdt1 = Utils::lorentzForce(charge, dxdt1, em);

    em = field.get(position + dxdt1 * halfStep, time + halfStep);
    Vector3<double> dxdt2 = Utils::velocity(momentum + dpdt1 * halfStep, mass);
    Vector3<double> dpdt2 = Utils::lorentzForce(charge, dxdt2, em);

    em = field.get(position + dxdt2 * halfStep, time + halfStep);
    Vector3<double> dxdt3 = Utils::velocity(momentum + dpdt2 * halfStep, mass);
    Vector3<double> dpdt3 = Utils::lorentzForce(charge, dxdt3, em);

    em = field.get(position + dxdt3 * dt, time + dt);
    Vector3<double> dxdt4 = Utils::velocity(momentum + dpdt3 * dt, mass);
    Vector3<double> dpdt4 = Utils::lorentzForce(charge, dxdt4, em);

    position += (dxdt1 + 2. * dxdt2 + 2. * dxdt3 + dxdt4) * (dt / 6.);
    momentum += (dpdt1 + 2. * dpdt2 + 2. * dpdt3 + dpdt4) * (dt / 6.);
};

REGISTER_PARTICLE(LeapfrogParticle)

LeapfrogParticle::LeapfrogParticle(const Vector3<double> &_position,
                                   const Vector3<double> &_momentum,
                                   double _charge, double _mass)
    : Particle(_position, _momentum, _charge, _mass){};

void LeapfrogParticle::nextStep(const Field &field, double time, double dt) {
    double halfStep = .5 * dt;
    position += Utils::velocity(momentum, mass) * halfStep;
    time += halfStep;
    EMField em = field.get(position, time);
    Vector3<double> pMinus = momentum + charge * halfStep * em.e;
    Vector3<double> t =
        charge * dt / (2. * mass * Utils::gamma(pMinus, mass)) * em.b;
    Vector3<double> s = 2. / (1. + t.square()) * t;
    Vector3<double> pPrime = pMinus + pMinus.cross(t);
    Vector3<double> pPlus = pMinus + pPrime.cross(s);
    momentum = pPlus + charge * halfStep * em.e;
    position += Utils::velocity(momentum, mass) * halfStep;
};