#include "Utils.hh"
#include <ctime>

std::default_random_engine Utils::e(time(NULL));
std::uniform_real_distribution<double> Utils::dist(0., 1.);