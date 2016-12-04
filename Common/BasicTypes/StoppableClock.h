
#ifndef __STOPPABLE_CLOCK__
#define __STOPPABLE_CLOCK__

#include "StringWrapper.h"
#ifndef _MSC_VER
#include <sys/time.h>
#else
#include <ctime>
#endif
#include <time.h>
#include <vector>
#include <functional>

class StoppableClock {
    protected:
        unsigned long long startTimeStamp;
        unsigned long long pausedWhen;
    public:
        StoppableClock() : StoppableClock(false) { }
        explicit StoppableClock(bool autoStart) {
            this->startTimeStamp = 0;
            this->pausedWhen = 0;
            if(autoStart) {
            	this->start();
            }
        }
        virtual ~StoppableClock() {
            this->startTimeStamp = 0;
            this->pausedWhen = 0;
        }
        
        StoppableClock& operator=(const StoppableClock& sc) {
            this->startTimeStamp = sc.startTimeStamp;
            this->pausedWhen = sc.pausedWhen;
            
            return (*this);
        }
        
        __inline unsigned long long operator-(const StoppableClock& sc) const {
            return this->startTimeStamp - sc.startTimeStamp;
        }
        
		void start();
        
        virtual unsigned long long stop() {
            unsigned long long res = this->getDuration();
            this->startTimeStamp = this->pausedWhen = 0;
          
            return res;
        }
        
		virtual unsigned long long timeLap();
        
        __inline bool isRunning() const {
            return this->pausedWhen == 0 && this->startTimeStamp > 0;
        }
        
        __inline void pause() {
			this->pausedWhen = StoppableClock::GetCurrentTimeStamp();
        }
        
        __inline void unpause() {
			this->startTimeStamp = StoppableClock::GetCurrentTimeStamp() - (this->pausedWhen - this->startTimeStamp);
            this->pausedWhen = 0;
        }
        
		unsigned long long getDuration() const;

		static unsigned long long GetCurrentTimeStamp();
        
        virtual String toString() const {
            return String("Currently at: ") + String::fromLong(StoppableClock::GetCurrentTimeStamp()) +
                    String(" | Started at: ") + String::fromLong(this->startTimeStamp) 
                    + String(" | Duration: ") + String::fromLong(this->getDuration()) 
                    + String("ms");
        }
};

class WrappingStoppableClock : public StoppableClock {
private:
	unsigned long long durationUntilWrapping;
	bool softStopFlag;
	std::function<void()> onTimeWrap;
	unsigned long long checkForTimeWrapping(unsigned long long tmpDuration) {
		while (tmpDuration >= durationUntilWrapping) {
			tmpDuration -= durationUntilWrapping;
			this->onTimeWrap();
			if (this->softStopFlag) {
				this->stop();
			}
		}
		return tmpDuration;
	}
public:
	WrappingStoppableClock() : WrappingStoppableClock(false) { }
	explicit WrappingStoppableClock(bool autoStart) : WrappingStoppableClock(autoStart, -1) {	}
	WrappingStoppableClock(bool autoStart, unsigned long long wrappingTime) : StoppableClock(autoStart) {
		this->setWrappingTime(wrappingTime);
		this->softStopFlag = false;
		this->onTimeWrap = []() {};
	}

	virtual ~WrappingStoppableClock() {}

	virtual unsigned long long timeLap() {
		unsigned long long realDuration = StoppableClock::timeLap();
		unsigned long long tmpDuration = checkForTimeWrapping(realDuration);
		if (this->isRunning()) {
			this->startTimeStamp = StoppableClock::GetCurrentTimeStamp() - tmpDuration;
		}
		return tmpDuration;
	}

	__inline unsigned long long getWrappingTime() const {
		return this->durationUntilWrapping;
	}
	__inline void setWrappingTime(const unsigned long long time) {
		durationUntilWrapping = time;
	}

	virtual unsigned long long stop() {
		this->softStopFlag = false;
		return StoppableClock::stop();
	}

	virtual void start() {
		StoppableClock::start();
		this->softStopFlag = false;
	}

	__inline void softStop() {
		this->softStopFlag = true;
	}

	virtual unsigned long long getDuration() {
		unsigned long long realDuration = StoppableClock::getDuration();
		unsigned long long tmpDuration = checkForTimeWrapping(realDuration);
		if (this->isRunning()) {
			this->startTimeStamp = StoppableClock::GetCurrentTimeStamp() - tmpDuration;
		}
		return tmpDuration;
	}

	__inline void setOnTimeWrap(std::function<void()> f) {
		this->onTimeWrap = (f == nullptr ? [](){} : f);
	}
};

class WrappingTriggerClock : public WrappingStoppableClock {
private:
	std::vector<std::function<void()>> trigger;
	std::vector<unsigned long long> triggerTimes;
	unsigned long currentTrigger;

	void checkForTrigger(const unsigned long long duration) {
		while (currentTrigger < triggerTimes.size() && triggerTimes[currentTrigger] < duration) {
			trigger[currentTrigger]();
			currentTrigger++;
		}
	}
public:
	WrappingTriggerClock() : WrappingTriggerClock(false) { }
	explicit WrappingTriggerClock(bool autoStart) : WrappingTriggerClock(autoStart, -1) {	}
	WrappingTriggerClock(bool autoStart, unsigned long long wrappingTime) : WrappingStoppableClock(autoStart, wrappingTime) {
		this->setOnTimeWrap([this]() {
			for (unsigned int i = currentTrigger; i < trigger.size(); i++) {
				this->trigger[i]();
			}
			this->currentTrigger = 0;
		});
	}
	virtual void start() {
		this->start(0);
	}
	void start(unsigned long long offset) {
		WrappingStoppableClock::start();
		this->startTimeStamp -= offset;
		this->currentTrigger = 0;
	}
	
	virtual unsigned long long timeLap() {
		unsigned long long realDuration = WrappingStoppableClock::timeLap();
		checkForTrigger(realDuration);
		return realDuration;
	}

	void update() {
		this->getDuration();
	}

	virtual unsigned long long getDuration() {
		unsigned long long dur = WrappingStoppableClock::getDuration();
		checkForTrigger(dur);
		return dur;
	}

	__inline void addTrigger(const unsigned long long triggerTime, std::function<void()> trigger) {
		this->trigger.push_back(trigger == nullptr ? [](){} : trigger);
		this->triggerTimes.push_back(triggerTime);
	}
};

#endif
