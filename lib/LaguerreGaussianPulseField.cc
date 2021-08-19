#include "LaguerreGaussianPulseField.hh"

#include "Config.hh"
#include "Utils.hh"

using namespace std;

REGISTER_MULTITON(Field, LaguerreGaussianPulseField)

LaguerreGaussianPulseField::LaguerreGaussianPulseField()
    : LaguerreGaussianPulseField(
          Config::getDouble(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_A0)),
          Config::getDouble(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_TAU)),
          Config::getDouble(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_W0)),
          Config::getDouble(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_EY)),
          Config::getDouble(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_DELAY)),
          Config::getInt(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_P)),
          Config::getInt(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_L)),
          Config::getDouble(SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_H),
                            0.5),
          Config::getDouble(
              SOVOL_CONFIG_KEY(LAGUERREGAUSSIANPULSEFIELD_IPHASE))){};

LaguerreGaussianPulseField::LaguerreGaussianPulseField(
    double _a0, double _tau, double _w0, double _ey, double _delay, int _p,
    int _l, double _h, double _iphase, double _k)
    : a0(std::fabs(_a0) * std::sqrt((double)Utils::factorial(_p) /
                                    Utils::factorial(_p + std::abs(_l)))),
      tau(std::fabs(_tau)),
      w0(std::fabs(_w0)),
      zR(0.5 * std::pow(w0, 2)),
      ex(std::sqrt(1. - std::pow(_ey, 2))),
      ey(_ey),
      delay(_delay),
      p(std::abs(_p)),
      l(_l),
      h(std::fabs(_h)),
      iphase(_iphase),
      k(_k){};

complex<double> LaguerreGaussianPulseField::amplitude(double x, double y,
                                                      double z) const {
  double r2 = std::pow(x, 2) + std::pow(y, 2);
  double wz = w0 * std::sqrt(1. + std::pow(z, 2) / (std::pow(zR, 2)));
  double irz = z / (std::pow(z, 2) + std::pow(zR, 2));
  double psiz = (std::abs(l) + 2 * p + 1) * std::atan(z / zR);
  double item1 = l == 0 ? 1. : std::pow(std::sqrt(2. * r2) / wz, std::abs(l));
  double item2 =
      p == 0 ? 1.
             : Utils::generLaguePoly(std::abs(l), p, 2. * r2 / std::pow(wz, 2));
  double extraPhase = l == 0 ? 0. : l * std::atan2(y, x);
  return a0 * (w0 / wz) * item1 * item2 * std::exp(-r2 / std::pow(wz, 2)) *
         std::exp(-1.i * (.5 * k * r2 * irz + extraPhase - psiz));
};

double LaguerreGaussianPulseField::envelope(double x, double y, double z,
                                            double t) const {
  double r2 = std::pow(x, 2) + std::pow(y, 2);
  double irz = z / (std::pow(z, 2) + std::pow(zR, 2));
  double phase = (t - delay) - (z + .5 * r2 * irz);
  return 1. / std::cosh(std::log(3. + 2. * std::sqrt(2)) * phase / tau);
};

EMField<double> LaguerreGaussianPulseField::operator()(double x, double y,
                                                       double z,
                                                       double t) const {
  complex<double> u = amplitude(x, y, z);
  complex<double> pupx =
      (amplitude(x + h, y, z) - amplitude(x - h, y, z)) / (2. * h);
  complex<double> pupy =
      (amplitude(x, y + h, z) - amplitude(x, y - h, z)) / (2. * h);
  complex<double> common =
      std::exp(1.i * ((t - z) * k + iphase)) * envelope(x, y, z, t);
  complex<double> Ex = -1.i * ex * u * common;
  complex<double> Ey = ey * u * common;
  complex<double> Ez = -(ex * pupx + ey * pupy * 1.i) * common;
  complex<double> Bx = -ey * u * common;
  complex<double> By = -1.i * ex * u * common;
  complex<double> Bz = (ey * pupx * 1.i - ex * pupy) * common;
  return EMField<double>{Vector3(Ex.real(), Ey.real(), Ez.real()),
                         Vector3(Bx.real(), By.real(), Bz.real())};
};