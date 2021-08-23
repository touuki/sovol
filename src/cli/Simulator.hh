#ifndef _SOVOL_SIMULATOR_HH
#define _SOVOL_SIMULATOR_HH 1

#include <map>
#include <mutex>
#include <vector>

#include "Algorithm.hh"
#include "Field.hh"
#include "H5Cpp.h"

class Simulator {
 private:
  std::shared_ptr<Field> field;
  std::shared_ptr<Algorithm> algorithm;
  std::vector<hsize_t> timePoints;
  double timeStep;

 public:
  Simulator(const std::shared_ptr<Field> _field,
            const std::shared_ptr<Algorithm> _algorithm,
            const std::vector<hsize_t> &_timePoints, double _timeStep);
  ~Simulator();
  std::map<std::string, std::vector<double>> operator()(
      std::shared_ptr<Particle> particle) const;
};

#endif