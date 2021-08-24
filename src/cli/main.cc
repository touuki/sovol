#include <unistd.h>

#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <thread>

#include "BoundedBuffer.hh"
#include "ParticleShifter.hh"
#include "Simulator.hh"
#include "Storage.hh"

#define TIME_FORMAT_STRING "%Y-%m-%d %H:%M:%S"

int main(int argc, char *const argv[]) {
  Lua::path = argv[1];
  Lua &lua = Lua::getInstance();
  lua.visitGlobal("control");
  int total_particle_number = lua.getField<int>("total_particle_number", 1);
  double time_step = lua.getField<double>("time_step");
  double data_start_time = lua.getField<double>("data_start_time", 0.);
  double data_end_time = lua.getField<double>("data_end_time");
  int data_interval = lua.getField<int>("data_interval");
  int parallel_workers = lua.getField<int>("parallel_workers", 1);
  std::stringstream default_filename_ss;
  default_filename_ss << "result-p" << getpid() << ".h5";
  std::string output_filename =
      lua.getField("output_filename", default_filename_ss.str());
  std::vector<std::string> output_items{
      "x",  "y",  "z",  "px", "py", "pz", "sx", "sy",
      "sz", "Ex", "Ey", "Ez", "Bx", "By", "Bz", "optical_depth"};
  output_items = lua.getField("output_items", output_items);
  lua.leaveTable();

  // auto data_interval for 0
  if (data_interval <= 0) {
    data_interval = (data_end_time - data_start_time) / time_step;
  }
  hsize_t length =
      (data_end_time - data_start_time) / time_step / data_interval + 1;

  std::vector<hsize_t> timePoints(length);
  hsize_t step = data_start_time / time_step;
  for (hsize_t i = 0; i < length; i++, step += data_interval) {
    timePoints[i] = step;
  }

  H5Storage storage(output_filename, output_items, length);

  double *times = new double[length];
  for (hsize_t i = 0; i < length; i++) {
    times[i] = time_step * timePoints[i];
  }
  storage.writeTime(times);
  delete[] times;

  Simulator simulator(lua.getGlobal<std::shared_ptr<Field>>("field"),
                      lua.getGlobal<std::shared_ptr<Algorithm>>("algorithm"),
                      output_items, std::move(timePoints), time_step);

  BoundedBuffer<std::shared_ptr<Particle>> particleBuffer(2 * parallel_workers,
                                                          1);
  BoundedBuffer<std::map<std::string, std::vector<double>>> resultBuffer(
      2 * parallel_workers, parallel_workers);

  std::thread storage_thread([&resultBuffer, &storage] {
    int count = 0;
    time_t last = time(NULL);
    std::cout << "[" << std::put_time(localtime(&last), TIME_FORMAT_STRING)
              << "] Start simulation." << std::endl;
    try {
      while (true) {
        time_t now = time(NULL);
        if (now - last > 60) {
          std::cout << "[" << std::put_time(localtime(&now), TIME_FORMAT_STRING)
                    << "] Finished " << count << " Particles." << std::endl;
          last = now;
        }
        storage.write(resultBuffer.pop());
        count++;
      }
    } catch (const NoProducerException &e) {
    }

    last = time(NULL);
    std::cout << "[" << std::put_time(localtime(&last), TIME_FORMAT_STRING)
              << "] Finish simulation. Total particles is " << count
              << std::endl;
  });

  std::vector<std::thread> simulator_threads;
  for (int i = 0; i < parallel_workers; i++) {
    simulator_threads.push_back(std::thread([&particleBuffer, &resultBuffer,
                                             &simulator] {
      utils::e.seed(std::hash<std::thread::id>{}(std::this_thread::get_id()));
      try {
        while (true) {
          resultBuffer.push(simulator(particleBuffer.pop()));
        }
      } catch (const NoProducerException &e) {
      }
      resultBuffer.producerQuit();
    }));
  }

  Particle particle = lua.getGlobal<Particle>("particle");
  std::vector<std::shared_ptr<ParticleShifter>> particle_shifters;
  particle_shifters = lua.getGlobal("particle_shifters", particle_shifters);
  for (int i = 0; i < total_particle_number; i++) {
    std::shared_ptr<Particle> p = std::make_shared<Particle>(particle);
    for (auto &&shifter : particle_shifters) {
      shifter->operator()(*p);
    }
    particleBuffer.push(p);
  }
  particleBuffer.producerQuit();

  storage_thread.join();
  for (auto &&t : simulator_threads) {
    t.join();
  }

  return 0;
}