#ifndef _SOVOL_LAGUERREGAUSSIANPULSEFIELD_HH
#define _SOVOL_LAGUERREGAUSSIANPULSEFIELD_HH 1

#include <complex>

#include "Field.hh"

class LaguerreGaussianPulseField : public Field {
 private:
  //这个a0理解为能量归一化参数
  //它使得相同w0和tau不同模式的激光归一化到相同的能量
  //只有对于基态模式它才代表腰处的矢势强度
  double a0;
  double tau;
  double w0;
  double zR;
  double ex;
  double ey;
  double delay;
  int p;
  int l;
  double h;
  double iphase;
  double k; // 2 * PI / lambda

  std::complex<double> amplitude(double x, double y, double z) const;
  double envelope(double x, double y, double z, double time) const;

 public:
  LaguerreGaussianPulseField();
  LaguerreGaussianPulseField(double _a0, double _tau, double _w0,
                             double _ey = 0., double _delay = 0., int _p = 0,
                             int _l = 0, double _h = 0.5, double _iphase = 0,
                             double _k = 1.);
  EMField<double> operator()(double x, double y, double z, double time) const override;
};

#endif