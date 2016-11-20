#ifndef __RING_COUNTER__
#define __RING_COUNTER__

#ifdef _MSC_VER
#pragma once
#endif

#include <algorithm>

class RingCounter {
private:
	unsigned long max;
	unsigned long current;
	__inline void progressCurrent() {
		current = (++current) % this->max;
	}
public:
	RingCounter() : RingCounter(1) {}
	RingCounter(unsigned long newMax) {
		this->max = (std::max)(1UL, newMax);
		this->current = 0;
	}

	virtual ~RingCounter() {}

	RingCounter& operator=(const RingCounter& other) = default;

	__inline unsigned long getCurrent() const {
		return this->current;
	}
	__inline unsigned long getMaximum() const {
		return this->max;
	}
	RingCounter& operator++() {
		this->progressCurrent();
		return (*this);
	}
	RingCounter operator++(int) {
		RingCounter tmp(*this);
		this->progressCurrent();
		return tmp;
	}
	operator unsigned long() const {
		return this->current;
	}
};

#endif