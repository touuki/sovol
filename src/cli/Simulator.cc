#include "Simulator.hh"

#include "ProParticle.hh"

Simulator::Simulator(const std::shared_ptr<Field> _field,
                     const std::shared_ptr<Algorithm> _algorithm,
                     const std::vector<hsize_t> &_timePoints, double _timeStep)
    : field(_field),
      algorithm(_algorithm),
      timePoints(_timePoints),
      timeStep(_timeStep){};
Simulator::~Simulator() {}

std::map<std::string, std::vector<double>> Simulator::operator()(
    std::shared_ptr<Particle> particle) const {
  Spinable *s = dynamic_cast<Spinable *>(particle.get());
  std::map<std::string, std::vector<double>> map;
  hsize_t length = timePoints.size();
  map["x"].resize(length);
  map["y"].resize(length);
  map["z"].resize(length);
  map["px"].resize(length);
  map["py"].resize(length);
  map["pz"].resize(length);
  if (s != nullptr) {
    map["sx"].resize(length);
    map["sy"].resize(length);
    map["sz"].resize(length);
  }
  for (hsize_t i = 0, j = 0; j < length; i++) {
    if (i >= timePoints[j]) {
      map["x"][j] = particle->position.x;
      map["y"][j] = particle->position.y;
      map["z"][j] = particle->position.z;
      map["px"][j] = particle->momentum.x;
      map["py"][j] = particle->momentum.y;
      map["pz"][j] = particle->momentum.z;
      if (s != nullptr) {
        map["sx"][j] = s->spin.x;
        map["sy"][j] = s->spin.y;
        map["sz"][j] = s->spin.z;
      }

      j++;
    }
    (*algorithm)(particle.get(), *field, timeStep * i, timeStep);
  }

  return map;
}