#include "utils.hh"

#include <fstream>

namespace utils {

namespace tables {
std::shared_ptr<Table1d> anomalousMagneticMoment;
std::shared_ptr<Table1d> photonEmissionRate;
std::shared_ptr<Table1d> photonEmissionRateSpinCorrection;
std::shared_ptr<Table3d> photonParameter;
std::shared_ptr<Table1d> k13;
std::shared_ptr<Table1d> k23;
std::shared_ptr<Table1d> intK13;
std::shared_ptr<Table1d> f1;
std::shared_ptr<Table1d> f2;
std::shared_ptr<Table1d> f3;
#ifdef __EMSCRIPTEN__
void init(emscripten::val tables) {
  anomalousMagneticMoment = Table1d::construct(std::istringstream(
      tables["anomalousMagneticMoment"]["arraybuffer"].as<std::string>()));
  photonEmissionRate = Table1d::construct(std::istringstream(
      tables["photonEmissionRate"]["arraybuffer"].as<std::string>()));
  photonEmissionRateSpinCorrection = Table1d::construct(std::istringstream(
      tables["photonEmissionRateSpinCorrection"]["arraybuffer"]
          .as<std::string>()));
  intK13 = Table1d::construct(
      std::istringstream(tables["intK13"]["arraybuffer"].as<std::string>()));
  k13 = Table1d::construct(
      std::istringstream(tables["k13"]["arraybuffer"].as<std::string>()));
  k23 = Table1d::construct(
      std::istringstream(tables["k23"]["arraybuffer"].as<std::string>()));
  photonParameter = Table3d::construct(std::istringstream(
      tables["photonParameter"]["arraybuffer"].as<std::string>()));
  f1 = Table1d::construct(
      std::istringstream(tables["f1"]["arraybuffer"].as<std::string>()));
  f2 = Table1d::construct(
      std::istringstream(tables["f2"]["arraybuffer"].as<std::string>()));
  f3 = Table1d::construct(
      std::istringstream(tables["f3"]["arraybuffer"].as<std::string>()));
};
#else
void init(const std::string &table_dirname) {
  anomalousMagneticMoment = Table1d::construct(
      std::ifstream(table_dirname + "/anomalousMagneticMoment.data"));
  photonEmissionRate = Table1d::construct(
      std::ifstream(table_dirname + "/photonEmissionRate.data"));
  photonEmissionRateSpinCorrection = Table1d::construct(
      std::ifstream(table_dirname + "/photonEmissionRateSpinCorrection.data"));
  intK13 = Table1d::construct(std::ifstream(table_dirname + "/intK13.data"));
  k13 = Table1d::construct(std::ifstream(table_dirname + "/k13.data"));
  k23 = Table1d::construct(std::ifstream(table_dirname + "/k23.data"));
  photonParameter = Table3d::construct(
      std::ifstream(table_dirname + "/photonParameter.data"));
  f1 = Table1d::construct(std::ifstream(table_dirname + "/f1.data"));
  f2 = Table1d::construct(std::ifstream(table_dirname + "/f2.data"));
  f3 = Table1d::construct(std::ifstream(table_dirname + "/f3.data"));
};
#endif
};  // namespace tables

std::uniform_real_distribution<double> uniform_dist;
std::normal_distribution<double> normal_dist;
thread_local std::mt19937 e(
    time(NULL) * std::hash<std::thread::id>{}(std::this_thread::get_id()));
}  // namespace utils