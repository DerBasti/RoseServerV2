#include "Randomizer.h"

std::minstd_rand0 Randomize::generator(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));