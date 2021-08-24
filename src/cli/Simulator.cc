#include "Simulator.hh"

#define S_OUTPUT_X (1 << 0)
#define S_OUTPUT_Y (1 << 1)
#define S_OUTPUT_Z (1 << 2)
#define S_OUTPUT_PX (1 << 3)
#define S_OUTPUT_PY (1 << 4)
#define S_OUTPUT_PZ (1 << 5)
#define S_OUTPUT_SX (1 << 6)
#define S_OUTPUT_SY (1 << 7)
#define S_OUTPUT_SZ (1 << 8)
#define S_OUTPUT_EX (1 << 9)
#define S_OUTPUT_EY (1 << 10)
#define S_OUTPUT_EZ (1 << 11)
#define S_OUTPUT_BX (1 << 12)
#define S_OUTPUT_BY (1 << 13)
#define S_OUTPUT_BZ (1 << 14)
#define S_OUTPUT_OPTICAL_DEPTH (1 << 15)

static uint16_t convertToFlag(const std::string &s) {
  if (s == "x") return S_OUTPUT_X;
  if (s == "y") return S_OUTPUT_Y;
  if (s == "z") return S_OUTPUT_Z;
  if (s == "px") return S_OUTPUT_PX;
  if (s == "py") return S_OUTPUT_PY;
  if (s == "pz") return S_OUTPUT_PZ;
  if (s == "sx") return S_OUTPUT_SX;
  if (s == "sy") return S_OUTPUT_SY;
  if (s == "sz") return S_OUTPUT_SZ;
  if (s == "Ex") return S_OUTPUT_EX;
  if (s == "Ey") return S_OUTPUT_EY;
  if (s == "Ez") return S_OUTPUT_EZ;
  if (s == "Bx") return S_OUTPUT_BX;
  if (s == "By") return S_OUTPUT_BY;
  if (s == "Bz") return S_OUTPUT_BZ;
  if (s == "optical_depth") return S_OUTPUT_OPTICAL_DEPTH;
  return 0;
}

static void initMap(std::map<std::string, std::vector<double>> &map,
                    hsize_t length, uint16_t flag) {
  if (flag & S_OUTPUT_X) map["x"].resize(length);
  if (flag & S_OUTPUT_Y) map["y"].resize(length);
  if (flag & S_OUTPUT_Z) map["z"].resize(length);
  if (flag & S_OUTPUT_PX) map["px"].resize(length);
  if (flag & S_OUTPUT_PY) map["py"].resize(length);
  if (flag & S_OUTPUT_PZ) map["pz"].resize(length);
  if (flag & S_OUTPUT_SX) map["sx"].resize(length);
  if (flag & S_OUTPUT_SY) map["sy"].resize(length);
  if (flag & S_OUTPUT_SZ) map["sz"].resize(length);
  if (flag & S_OUTPUT_EX) map["Ex"].resize(length);
  if (flag & S_OUTPUT_EY) map["Ey"].resize(length);
  if (flag & S_OUTPUT_EZ) map["Ez"].resize(length);
  if (flag & S_OUTPUT_BX) map["Bx"].resize(length);
  if (flag & S_OUTPUT_BY) map["By"].resize(length);
  if (flag & S_OUTPUT_BZ) map["Bz"].resize(length);
  if (flag & S_OUTPUT_OPTICAL_DEPTH) map["optical_depth"].resize(length);
}

static void setMapData(std::map<std::string, std::vector<double>> &map,
                       const Particle &particle, hsize_t index, uint16_t flag) {
  if (flag & S_OUTPUT_X) map["x"][index] = particle.position.x;
  if (flag & S_OUTPUT_Y) map["y"][index] = particle.position.y;
  if (flag & S_OUTPUT_Z) map["z"][index] = particle.position.z;
  if (flag & S_OUTPUT_PX) map["px"][index] = particle.momentum.x;
  if (flag & S_OUTPUT_PY) map["py"][index] = particle.momentum.y;
  if (flag & S_OUTPUT_PZ) map["pz"][index] = particle.momentum.z;
  if (flag & S_OUTPUT_SX) map["sx"][index] = particle.polarization.x;
  if (flag & S_OUTPUT_SY) map["sy"][index] = particle.polarization.y;
  if (flag & S_OUTPUT_SZ) map["sz"][index] = particle.polarization.z;
  if (flag & S_OUTPUT_EX) map["Ex"][index] = particle.em_field.E.x;
  if (flag & S_OUTPUT_EY) map["Ey"][index] = particle.em_field.E.y;
  if (flag & S_OUTPUT_EZ) map["Ez"][index] = particle.em_field.E.z;
  if (flag & S_OUTPUT_BX) map["Bx"][index] = particle.em_field.B.x;
  if (flag & S_OUTPUT_BY) map["By"][index] = particle.em_field.B.y;
  if (flag & S_OUTPUT_BZ) map["Bz"][index] = particle.em_field.B.z;
  if (flag & S_OUTPUT_OPTICAL_DEPTH)
    map["optical_depth"][index] = particle.optical_depth;
}

Simulator::Simulator(const std::shared_ptr<Field> &_field,
                     const std::shared_ptr<Algorithm> &_algorithm,
                     const std::vector<std::string> &_outputItems,
                     const std::vector<hsize_t> &_timePoints, double _timeStep)
    : field(_field),
      algorithm(_algorithm),
      timePoints(_timePoints),
      timeStep(_timeStep) {
  for (auto &&item : _outputItems) {
    outputFlag |= convertToFlag(item);
  }
};

std::map<std::string, std::vector<double>> Simulator::operator()(
    const std::shared_ptr<Particle> &particle) const {
  std::map<std::string, std::vector<double>> map;
  hsize_t length = timePoints.size();
  initMap(map, length, outputFlag);
  for (hsize_t i = 0, j = 0; j < length; i++) {
    if (i >= timePoints[j]) {
      setMapData(map, *particle, j, outputFlag);
      j++;
    }
    algorithm->operator()(*particle, *field, timeStep *i, timeStep);
  }

  return map;
}