#include "Utils.hh"

std::uniform_real_distribution<double> Utils::uniform_dist;
std::normal_distribution<double> Utils::normal_dist;
thread_local std::mt19937 Utils::e(
    time(NULL) + std::hash<std::thread::id>{}(std::this_thread::get_id()));
