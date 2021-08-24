#include "utils.hh"

std::uniform_real_distribution<double> utils::uniform_dist;
std::normal_distribution<double> utils::normal_dist;
thread_local std::mt19937 utils::e(
    time(NULL) + std::hash<std::thread::id>{}(std::this_thread::get_id()));
