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

std::chrono::steady_clock::time_point release_control(
    int local_id, long sleep_millisecond = 0) {
  emscripten_sleep(sleep_millisecond);
  if (local_id != simulation_id || !is_start) throw InterruptException();
  return std::chrono::steady_clock::now();
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
  double data_interval = control["data_interval"].as<double>();
  double data_start_time = control["data_start_time"].as<double>();
  emscripten::val callback = val::global("simulation_callback");

  std::chrono::steady_clock::time_point begin = release_control(local_id);
  try {
    for (int i = 0; i < total_particle_number; i++) {
      Particle particle = particle_producer();
      val result = val::object();
      result.set("type", "new_particle");
      callback(result);
      begin = release_control(local_id);

      double time = 0;
      for (double next_data_time = data_start_time; time < data_end_time;
           time += time_step) {
        if (time >= next_data_time) {
          result = val::object();
          result.set("type", "data");
          result.set("time", time);
          result.set("particle", particle.toEmscriptenVal());
          callback(result);
          begin = release_control(local_id);

          next_data_time =
              std::max(next_data_time + data_interval, time + time_step);
        } else if (std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::steady_clock::now() - begin)
                       .count() > 1000) {
          begin = release_control(local_id);
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