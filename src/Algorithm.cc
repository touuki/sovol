#include "Algorithm.hh"

#include "utils.hh"

class FrequencyHelper {
 public:
  double frequency;
  double criticalField;
  double comptonTime;
  double emissionFactor;
  double classicalRadiatedFactor;

  FrequencyHelper(double _frequency)
      : frequency(_frequency),
        criticalField((CONST_M0 * CONST_C2 / CONST_HBAR) / frequency),
        comptonTime(CONST_HBAR / (CONST_M0 * CONST_C2) * frequency),
        emissionFactor(CONST_ALPHA * CONST_SQRT3 / (2. * M_PI) / comptonTime),
        classicalRadiatedFactor(2. * CONST_ALPHA / (3. * comptonTime)){};
};

class NoneAlgorithm : public Algorithm {
 public:
  void operator()(Particle &, const Field &, double time,
                  double dt) const override{};
};

REGISTER_SINGLETON(Algorithm, NoneAlgorithm)

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
 protected:
  virtual double anomalousMagneticMoment(const Vector3<double> &,
                                         const EMField<double> &) const {
    return CONST_ALPHA / (2. * M_PI);
  };

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

    Vector3<double> pNext = pPlus + part.charge * halfStep * em.E;
    if (part.type == electron) {
      Vector3<double> pHalf = (part.momentum + pNext) / 2.;
      double gamma = utils::gamma(pHalf, part.mass);
      Vector3<double> beta = utils::velocity(pHalf, part.mass);
      double a = anomalousMagneticMoment(pHalf, em);
      Vector3<double> Omega = (a + 1. / gamma) * em.B -
                              a * gamma / (gamma + 1.) * beta.dot(em.B) * beta -
                              (a + 1. / (gamma + 1.)) * beta.cross(em.E);
      t = -Omega * halfStep;
      s = 2. / (1. + t.square()) * t;
      part.polarization +=
          (part.polarization + part.polarization.cross(t)).cross(s);
    }

    part.em_field = em;
    part.momentum = pNext;
    part.position += utils::velocity(part.momentum, part.mass) * halfStep;
  }
};

REGISTER_SINGLETON(Algorithm, LeapfrogAlgorithm)

class ModifiedAMMLeapfrogAlgorithm : public LeapfrogAlgorithm,
                                     public FrequencyHelper {
 protected:
  double anomalousMagneticMoment(
      const Vector3<double> &momentum,
      const EMField<double> &emField) const override {
    Vector3<double> beta = utils::velocity(momentum, 1.);
    double chi_e = utils::gamma(momentum, 1.) *
                   std::sqrt((emField.E + beta.cross(emField.B)).square() -
                             std::pow(emField.E.dot(beta), 2)) /
                   criticalField;
    return utils::tables::anomalousMagneticMoment->operator()(
        std::log10(chi_e));
  };

 public:
  ModifiedAMMLeapfrogAlgorithm(double _frequency);
#ifdef __EMSCRIPTEN__
  ModifiedAMMLeapfrogAlgorithm(emscripten::val v)
      : FrequencyHelper(
            emscripten::val::global("reference_frequency").as<double>()){};
#else
  ModifiedAMMLeapfrogAlgorithm(Lua &lua)
      : FrequencyHelper(lua.getGlobal<double>("reference_frequency")){};
#endif
};

REGISTER_MULTITON(Algorithm, ModifiedAMMLeapfrogAlgorithm)

class RadiativeAlgorithm : public Algorithm, public FrequencyHelper {
 private:
  std::shared_ptr<Algorithm> base_algorithm;
  virtual void radiate(Particle &part, const Field &field, double time,
                       double dt) const = 0;

 public:
  RadiativeAlgorithm(double _frequency,
                     std::shared_ptr<Algorithm> _base_algorithm)
      : FrequencyHelper(_frequency), base_algorithm(_base_algorithm){};
  void operator()(Particle &part, const Field &field, double time,
                  double dt) const override {
    base_algorithm->operator()(part, field, time, dt);
    if (part.type == electron) radiate(part, field, time, dt);
  };
};

class MonteCarloRadiativePolarizationAlgorithm : public RadiativeAlgorithm {
 private:
  double min_chi_e;
  bool disable_reaction;
  bool disable_spin_effect;
  void radiate(Particle &part, const Field &field, double time,
               double dt) const override {
    static std::uniform_real_distribution<double> rand(0., 1.);
    EMField em = field(part.position, time);
    for (double emissionTime, localTime = 0.; localTime < dt;
         localTime += emissionTime) {
      double gamma = utils::gamma(part.momentum, part.mass);
      Vector3<double> beta = utils::velocity(part.momentum, part.mass);
      double chi_e = gamma *
                     std::sqrt((em.E + beta.cross(em.B)).square() -
                               std::pow(em.E.dot(beta), 2)) /
                     criticalField;
      if (chi_e <= min_chi_e) return;
      double log_chi_e = std::log10(chi_e);

      if (disable_spin_effect) {
        double temp = emissionFactor * (chi_e / gamma) *
                      utils::tables::photonEmissionRate->operator()(log_chi_e);
        emissionTime = std::min(part.optical_depth / temp, dt - localTime);
        part.optical_depth -= emissionTime * temp;
        if (part.optical_depth <= 1e-100) {
          double chi_gamma =
              std::pow(10, utils::tables::photonParameter->operator()(
                               log_chi_e, 0., utils::random()));
          double photonEnergy = (gamma - 1.) * chi_gamma / chi_e;
          Vector3<double> photonMomentum = photonEnergy * part.momentum.unit();
          // if (photonEnergy > minPhotonEnergy); // Produce photon
          if (!disable_reaction) part.momentum -= photonMomentum;
          part.resetOpticalDepth();
        }
      } else {
        Vector3<double> b_RF =
            gamma * (em.B - beta.cross(em.E) -
                     gamma / (gamma + 1.) * beta.dot(em.B) * beta);
        Vector3<double> zeta = b_RF.unit();
        double s_zeta = zeta.dot(part.polarization);
        double persc =
            utils::tables::photonEmissionRateSpinCorrection->operator()(
                log_chi_e);
        double ef = emissionFactor * (chi_e / gamma);

        double temp =
            ef * (utils::tables::photonEmissionRate->operator()(log_chi_e) -
                  s_zeta * persc);
        emissionTime = std::min(part.optical_depth / temp, dt - localTime);
        part.optical_depth -= emissionTime * temp;

        Vector3<double> d_spin =
            emissionTime * ef * persc *
            (zeta - zeta.dot(part.polarization) * part.polarization);
        if (part.optical_depth <= 1e-100) {
          double chi_gamma =
              std::pow(10, utils::tables::photonParameter->operator()(
                               log_chi_e, s_zeta, utils::random()));
          double x = chi_gamma / chi_e;
          double photonEnergy = (gamma - 1.) * x;
          Vector3<double> photonMomentum = photonEnergy * part.momentum.unit();
          // if (photonEnergy > minPhotonEnergy); // Produce photon
          if (!disable_reaction) part.momentum -= photonMomentum;
          part.resetOpticalDepth();

          double u = chi_gamma / (chi_e - chi_gamma);
          double u_p = (2. / 3.) * u / chi_e;
#ifdef __EMSCRIPTEN__
          double k13 = utils::tables::k13->operator()(std::log10(u_p));
          double k23 = utils::tables::k23->operator()(std::log10(u_p));
#else
          double k13 = std::cyl_bessel_k(1. / 3., u_p);
          double k23 = std::cyl_bessel_k(2. / 3., u_p);
#endif
          double intK13 = utils::tables::intK13->operator()(std::log10(u_p));

          Vector3<double> e_RF =
              gamma * (em.E + beta.cross(em.B) -
                       gamma / (gamma + 1.) * beta.dot(em.E) * beta);
          Vector3<double> eta = e_RF.unit();
          Vector3<double> kappa = eta.cross(zeta);

          double ifactor =
              1. / ((2. + x * u) * k23 - intK13 - x * s_zeta * k13);
          /*
          double spin_zeta =
              (s_zeta * (2. * k23 - intK13) - u * k13) * ifactor;
          double spin_eta =
              eta.dot(part.polarization) * (2. * k23 - intK13) * ifactor;
          double spin_kappa =
              kappa.dot(part.polarization) *
              ((2. + x * u) * k23 - (1. + x * u) * intK13) * ifactor;

          part.polarization =
              spin_zeta * zeta + spin_eta * eta + spin_kappa * kappa;
          */

          double d_spin_zeta =
              (-s_zeta * u * x * k23 - (u - std::pow(s_zeta, 2) * x) * k13) *
              ifactor;
          double d_spin_eta = -eta.dot(part.polarization) * x *
                              (u * k23 - s_zeta * k13) * ifactor;
          double d_spin_kappa = -kappa.dot(part.polarization) * x *
                                (u * intK13 - s_zeta * k13) * ifactor;
          part.polarization +=
              d_spin_zeta * zeta + d_spin_eta * eta + d_spin_kappa * kappa;
        }

        part.polarization += d_spin;
      }
    }
  };

 public:
  MonteCarloRadiativePolarizationAlgorithm(
      double _frequency, std::shared_ptr<Algorithm> _base_algorithm,
      double _min_chi_e = 0., bool _disable_reaction = false,
      bool _disable_spin_effect = false);
#ifdef __EMSCRIPTEN__
  MonteCarloRadiativePolarizationAlgorithm(emscripten::val v)
      : RadiativeAlgorithm(
            emscripten::val::global("reference_frequency").as<double>(),
            Algorithm::createObject(v["base_algorithm"])),
        min_chi_e(v["min_chi_e"].isUndefined() ? 0.
                                               : v["min_chi_e"].as<double>()),
        disable_reaction(v["disable_reaction"].isUndefined()
                             ? false
                             : v["disable_reaction"].as<bool>()),
        disable_spin_effect(v["disable_spin_effect"].isUndefined()
                                ? false
                                : v["disable_spin_effect"].as<bool>()){};
#else
  MonteCarloRadiativePolarizationAlgorithm(Lua &lua)
      : RadiativeAlgorithm(
            lua.getGlobal<double>("reference_frequency"),
            lua.getField<std::shared_ptr<Algorithm> >("base_algorithm")),
        min_chi_e(lua.getField<double>("min_chi_e", 0.)),
        disable_reaction(lua.getField("disable_reaction", false)),
        disable_spin_effect(lua.getField("disable_spin_effect", false)){};
#endif
};

REGISTER_MULTITON(Algorithm, MonteCarloRadiativePolarizationAlgorithm)

class ContinuousRadiativePolarizationAlgorithm : public RadiativeAlgorithm {
 private:
  double min_chi_e;
  bool disable_reaction;
  bool disable_spin_effect;
  void radiate(Particle &part, const Field &field, double time,
               double dt) const override {
    EMField em = field(part.position, time);
    double gamma = utils::gamma(part.momentum, part.mass);
    Vector3<double> beta = utils::velocity(part.momentum, part.mass);
    double chi_e = gamma *
                   std::sqrt((em.E + beta.cross(em.B)).square() -
                             std::pow(em.E.dot(beta), 2)) /
                   criticalField;
    if (chi_e <= min_chi_e) return;
    double log_chi_e = std::log10(chi_e);

    if (!disable_spin_effect) {
      Vector3<double> b_RF =
          gamma * (em.B - beta.cross(em.E) -
                   gamma / (gamma + 1.) * beta.dot(em.B) * beta);
      Vector3<double> e_RF =
          gamma * (em.E + beta.cross(em.B) -
                   gamma / (gamma + 1.) * beta.dot(em.E) * beta);
      Vector3<double> zeta = b_RF.unit();
      Vector3<double> eta = e_RF.unit();
      Vector3<double> kappa = eta.cross(zeta);
      double s_zeta = zeta.dot(part.polarization);
      double s_eta = eta.dot(part.polarization);
      double s_kappa = kappa.dot(part.polarization);

      double f1 = utils::tables::f1->operator()(log_chi_e);
      double f2 = utils::tables::f2->operator()(log_chi_e);
      double f3 = utils::tables::f3->operator()(log_chi_e);
      double factor = emissionFactor * (chi_e / gamma) * dt;

      double d_spin_zeta = factor * (-s_zeta * f2 - f1);
      double d_spin_eta = -factor * s_eta * f2;
      double d_spin_kappa = -factor * s_kappa * f3;
      part.polarization +=
          d_spin_zeta * zeta + d_spin_eta * eta + d_spin_kappa * kappa;
    }

    if (!disable_reaction) {
      part.momentum -= classicalRadiatedFactor * dt * std::pow(chi_e, 2) *
                       gamma / (std::pow(gamma, 2) - 1) * part.momentum;
    }
  };

 public:
  ContinuousRadiativePolarizationAlgorithm(
      double _frequency, std::shared_ptr<Algorithm> _base_algorithm,
      double _min_chi_e = 0., bool _disable_reaction = false,
      bool _disable_spin_effect = false);
#ifdef __EMSCRIPTEN__
  ContinuousRadiativePolarizationAlgorithm(emscripten::val v)
      : RadiativeAlgorithm(
            emscripten::val::global("reference_frequency").as<double>(),
            Algorithm::createObject(v["base_algorithm"])),
        min_chi_e(v["min_chi_e"].isUndefined() ? 0.
                                               : v["min_chi_e"].as<double>()),
        disable_reaction(v["disable_reaction"].isUndefined()
                             ? false
                             : v["disable_reaction"].as<bool>()),
        disable_spin_effect(v["disable_spin_effect"].isUndefined()
                                ? false
                                : v["disable_spin_effect"].as<bool>()){};
#else
  ContinuousRadiativePolarizationAlgorithm(Lua &lua)
      : RadiativeAlgorithm(
            lua.getGlobal<double>("reference_frequency"),
            lua.getField<std::shared_ptr<Algorithm> >("base_algorithm")),
        min_chi_e(lua.getField<double>("min_chi_e", 0.)),
        disable_reaction(lua.getField("disable_reaction", false)),
        disable_spin_effect(lua.getField("disable_spin_effect", false)){};
#endif
};

REGISTER_MULTITON(Algorithm, ContinuousRadiativePolarizationAlgorithm)
