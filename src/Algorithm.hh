#ifndef _SOVOL_ALGORITHM_HH
#define _SOVOL_ALGORITHM_HH 1

#include <vector>

#include "Particle.hh"

class Algorithm {
  DEFINE_FACTORY(Algorithm)
 public:
  virtual ~Algorithm(){};
  virtual void operator()(Particle &, const Field &, double time,
                          double dt) const = 0;
};

#endif