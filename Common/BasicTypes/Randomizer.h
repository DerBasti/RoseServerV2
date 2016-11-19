#ifndef __RANDOMIZER__
#define __RANDOMIZER__

#ifdef _MSC_VER
#pragma once
#endif

#include <random>
#include <chrono>

class Randomize {
private:
	Randomize() {}
	~Randomize() {}
	static std::minstd_rand0  generator;
public:
	static unsigned long getUInt() {
		return getUInt((std::numeric_limits<unsigned long>::max)());
	}
	static unsigned long getUInt(unsigned long max) {
		return getUInt(0, max);
	}
	static unsigned long getUInt(unsigned long min, unsigned long max) {
		std::uniform_int_distribution<unsigned long> distribution(min, max);
		return distribution(generator);
	}

	static float GetFloat() {
		return GetFloat((std::numeric_limits<float>::max)());
	}
	static float GetFloat(const float upperBoundry) {
		return GetFloat(0.0f, upperBoundry);
	}
	static float GetFloat(const float lower, const float upper) {
		std::uniform_real_distribution<float> distribution(lower, upper);
		return distribution(generator);
	}
};

#endif 