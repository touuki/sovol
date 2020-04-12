#include "Algorithm.hh"
#include "AlgorithmFactory.hh"
#include "Utils.hh"

REGISTER_ALGORITHM(RungeKuttaAlgorithm)

void RungeKuttaAlgorithm::operator()(Particle *part, const Field *field,
                                     double time, double dt) {
    double halfStep = .5 * dt;
    EMField em = field->get(part->position, time);
    Vector3<double> dxdt1 = Utils::velocity(part->momentum, part->mass);
    Vector3<double> dpdt1 = Utils::lorentzForce(part->charge, dxdt1, em);

    em = field->get(part->position + dxdt1 * halfStep, time + halfStep);
    Vector3<double> dxdt2 = Utils::velocity(part->momentum + dpdt1 * halfStep, part->mass);
    Vector3<double> dpdt2 = Utils::lorentzForce(part->charge, dxdt2, em);

    em = field->get(part->position + dxdt2 * halfStep, time + halfStep);
    Vector3<double> dxdt3 = Utils::velocity(part->momentum + dpdt2 * halfStep, part->mass);
    Vector3<double> dpdt3 = Utils::lorentzForce(part->charge, dxdt3, em);

    em = field->get(part->position + dxdt3 * dt, time + dt);
    Vector3<double> dxdt4 = Utils::velocity(part->momentum + dpdt3 * dt, part->mass);
    Vector3<double> dpdt4 = Utils::lorentzForce(part->charge, dxdt4, em);

    part->position += (dxdt1 + 2. * dxdt2 + 2. * dxdt3 + dxdt4) * (dt / 6.);
    part->momentum += (dpdt1 + 2. * dpdt2 + 2. * dpdt3 + dpdt4) * (dt / 6.);
};

REGISTER_ALGORITHM(LeapfrogAlgorithm)

void LeapfrogAlgorithm::operator()(Particle *part, const Field *field,
                                   double time, double dt) {
    double halfStep = .5 * dt;
    part->position += Utils::velocity(part->momentum, part->mass) * halfStep;
    time += halfStep;
    EMField em = field->get(part->position, time);
    Vector3<double> pMinus = part->momentum + part->charge * halfStep * em.e;
    Vector3<double> t =
        part->charge * dt / (2. * part->mass * Utils::gamma(pMinus, part->mass)) * em.b;
    Vector3<double> s = 2. / (1. + t.square()) * t;
    Vector3<double> pPrime = pMinus + pMinus.cross(t);
    Vector3<double> pPlus = pMinus + pPrime.cross(s);
    part->momentum = pPlus + part->charge * halfStep * em.e;
    part->position += Utils::velocity(part->momentum, part->mass) * halfStep;
};