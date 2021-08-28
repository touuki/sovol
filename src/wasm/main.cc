#include <emscripten.h>
#include <emscripten/bind.h>

#include <chrono>
#include <ctime>
#include <vector>

#include "Algorithm.hh"
#include "ParticleProducer.hh"
#include "utils.hh"

using namespace emscripten;

namespace wasm {

class InterruptException : public std::exception {
  const char* what() const throw() { return "InterruptException"; }
};

int simulation_id;
bool is_start;

double release_control(int local_id, unsigned int sleep_millisecond = 0) {
  emscripten_sleep(sleep_millisecond);
  if (local_id != simulation_id || !is_start) throw InterruptException();
  return val::global("Date").new_().call<double>("getTime");
}

void start_simulation() {
  is_start = true;
  int local_id = ++simulation_id;
  std::shared_ptr<Field> field = Field::createObject(val::global("field"));
  std::shared_ptr<Algorithm> algorithm =
      Algorithm::createObject(val::global("algorithm"));
  ParticleProducer particle_producer(val::global("particle_producer"));
  val control = val::global("control");
  double time_step = control["time_step"].as<double>();
  double data_end_time = control["data_end_time"].as<double>();
  int total_particle_number = control["total_particle_number"].as<int>();
  int data_interval = control["data_interval"].as<int>();
  double data_start_time = control["data_start_time"].as<double>();
  int data_per_frame = control["data_per_frame"].as<int>();
  int frames_per_second = control["frames_per_second"].as<int>();
  emscripten::val callback = val::global("simulation_callback");
  // auto data_interval for 0
  if (data_interval <= 0) {
    data_interval = (data_end_time - data_start_time) / time_step;
  }

  double begin = val::global("Date").new_().call<double>("getTime");
  try {
    for (int i = 0; i < total_particle_number; i++) {
      Particle particle = particle_producer();
      val result = val::object();
      result.set("type", "new_particle");
      callback(result);

      double time = 0, data_count = 0;
      for (double next_data_time = data_start_time; time < data_end_time;
           time += time_step) {
        if (time >= next_data_time) {
          result = val::object();
          result.set("type", "data");
          result.set("time", time);
          result.set("particle", particle.toEmscriptenVal());
          callback(result);

          data_count++;
          next_data_time += data_interval * time_step;
          if (data_count >= data_per_frame) {
            data_count = 0;
            result = val::object();
            result.set("type", "update_frame");
            callback(result);
            double now = val::global("Date").new_().call<double>("getTime");
            begin = release_control(
                local_id, std::max(static_cast<int>(
                                       begin + 1000 / frames_per_second - now),
                                   0));
          }
        }
        algorithm->operator()(particle, *field, time, time_step);
      }
      result = val::object();
      result.set("type", "end_particle");
      result.set("time", time);
      result.set("particle", particle.toEmscriptenVal());
      callback(result);
      begin = release_control(local_id);
    }
  } catch (const InterruptException& e) {
  }
  is_start = false;
};

void stop_simlation() { is_start = false; };

bool is_simulation_start() { return is_start; };

}  // namespace wasm

EMSCRIPTEN_BINDINGS(module) {
  function("initTables", &utils::tables::init);
  function("startSimulation", &wasm::start_simulation);
  function("stopSimulation", &wasm::stop_simlation);
  function("isSimulationStart", &wasm::is_simulation_start);
}