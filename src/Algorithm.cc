#include "Algorithm.hh"

#include "utils.hh"

class RungeKuttaAlgorithm : public Algorithm {
 public:
  void operator()(Particle &part, const Field &field, double time,
                  double dt) const override {
    double halfStep = .5 * dt;
    EMField em = field(part.position, time);
    part.em_field = em;
    Vector3<double> dxdt1 = utils::velocity(part.momentum, part.mass);
    Vector3<double> dpdt1 = utils::lorentzForce(part.charge, dxdt1, em);

    em = field(part.position + dxdt1 * halfStep, time + halfStep);
    Vector3<double> dxdt2 =
        utils::velocity(part.momentum + dpdt1 * halfStep, part.mass);
    Vector3<double> dpdt2 = utils::lorentzForce(part.charge, dxdt2, em);

    em = field(part.position + dxdt2 * halfStep, time + halfStep);
    Vector3<double> dxdt3 =
        utils::velocity(part.momentum + dpdt2 * halfStep, part.mass);
    Vector3<double> dpdt3 = utils::lorentzForce(part.charge, dxdt3, em);

    em = field(part.position + dxdt3 * dt, time + dt);
    Vector3<double> dxdt4 =
        utils::velocity(part.momentum + dpdt3 * dt, part.mass);
    Vector3<double> dpdt4 = utils::lorentzForce(part.charge, dxdt4, em);

    part.position += (dxdt1 + 2. * dxdt2 + 2. * dxdt3 + dxdt4) * (dt / 6.);
    part.momentum += (dpdt1 + 2. * dpdt2 + 2. * dpdt3 + dpdt4) * (dt / 6.);
  };
};

REGISTER_SINGLETON(Algorithm, RungeKuttaAlgorithm)

class LeapfrogAlgorithm : public Algorithm {
 public:
  void operator()(Particle &part, const Field &field, double time,
                  double dt) const override {
    double halfStep = .5 * dt;
    part.position += utils::velocity(part.momentum, part.mass) * halfStep;
    time += halfStep;
    EMField em = field(part.position, time);
    Vector3<double> pMinus = part.momentum + part.charge * halfStep * em.E;
    Vector3<double> t = part.charge * dt /
                        (2. * part.mass * utils::gamma(pMinus, part.mass)) *
                        em.B;
    Vector3<double> s = 2. / (1. + t.square()) * t;
    Vector3<double> pPrime = pMinus + pMinus.cross(t);
    Vector3<double> pPlus = pMinus + pPrime.cross(s);
    part.em_field = em;
    part.momentum = pPlus + part.charge * halfStep * em.E;
    part.position += utils::velocity(part.momentum, part.mass) * halfStep;
  };
};

REGISTER_SINGLETON(Algorithm, LeapfrogAlgorithm)