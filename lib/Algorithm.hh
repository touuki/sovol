#ifndef _SOVOL_ALGORITHM_HH
#define _SOVOL_ALGORITHM_HH 1

#include <vector>

#include "Field.hh"
#include "Particle.hh"

class Algorithm {
 public:
  virtual ~Algorithm();
  virtual void operator()(Particle *, const Field &, double time,
                          double dt) const = 0;
};

DEFINE_FACTORY(Algorithm)

class RungeKuttaAlgorithm : public Algorithm {
 public:
  void operator()(Particle *, const Field &, double time,
                  double dt) const override;
};

class LeapfrogAlgorithm : public Algorithm {
 public:
  void operator()(Particle *, const Field &, double time,
                  double dt) const override;
};

#endif