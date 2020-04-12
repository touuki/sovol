#ifndef _SOVOL_ALGORITHM_HH
#define _SOVOL_ALGORITHM_HH 1

#include "Field.hh"
#include "Particle.hh"
#include <vector>

class Algorithm {
  public:
    virtual ~Algorithm();
    virtual void operator()(Particle *, const Field *, double time, double dt) = 0;
};

class RungeKuttaAlgorithm : public Algorithm {
  public:
    void operator()(Particle *, const Field *, double time, double dt) override;
};

class LeapfrogAlgorithm : public Algorithm {
  public:
    void operator()(Particle *, const Field *, double time, double dt) override;
};
#endif