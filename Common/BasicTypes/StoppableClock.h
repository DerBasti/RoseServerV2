
#ifndef __STOPPABLE_CLOCK__
#define __STOPPABLE_CLOCK__

#include "StringWrapper.h"
#ifndef _MSC_VER
#include <sys/time.h>
#else
#include <ctime>
#endif
#include <time.h>

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
        ~StoppableClock() {
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
        
        void start() {
            this->pausedWhen = 0;
			this->startTimeStamp = StoppableClock::GetCurrentTimeStamp();
        }
        
        unsigned long long stop() {
            unsigned long long res = this->getDuration();
            this->startTimeStamp = 0;
            this->pausedWhen = 0;
          
            return res;
        }
        
        unsigned long long timeLap() {
            if(this->startTimeStamp == 0 || this->pausedWhen != 0) {
                return 0;
            }
            unsigned long long res = this->getDuration();
			this->startTimeStamp = StoppableClock::GetCurrentTimeStamp();
            
            return res;
        }
        
        __inline bool isRunning() const {
            return this->pausedWhen == 0 && this->startTimeStamp > 0;
        }
        
        __inline void pause() {
			this->pausedWhen = StoppableClock::GetCurrentTimeStamp();
        }
        
        void unpause() {
			this->startTimeStamp = StoppableClock::GetCurrentTimeStamp() - (this->pausedWhen - this->startTimeStamp);
            this->pausedWhen = 0;
        }
        
        __inline long long getDuration() const {
			return StoppableClock::GetCurrentTimeStamp() - this->startTimeStamp;
        }

		static long long GetCurrentTimeStamp();
        
        virtual String toString() const {
            return String("Currently at: ") + String::fromLong(StoppableClock::GetCurrentTimeStamp()) +
                    String(" | Started at: ") + String::fromLong(this->startTimeStamp) 
                    + String(" | Duration: ") + String::fromLong(this->getDuration()) 
                    + String("ms");
        }
};

#endif
