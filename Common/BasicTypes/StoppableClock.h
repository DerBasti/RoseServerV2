
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

		static long long GetCurrentTimeStamp() {
			long long result = 0x00;
#ifdef _MSC_VER
			SYSTEMTIME st;
			GetSystemTime(&st);

			std::tm tm;
			tm.tm_sec = st.wSecond;
			tm.tm_min = st.wMinute;
			tm.tm_hour = st.wHour;
			tm.tm_mday = st.wDay;
			tm.tm_mon = st.wMonth - 1;
			tm.tm_year = st.wYear - 1900;

			time_t timeStamp = std::mktime(&tm);
			result = (timeStamp * 1000) + st.wMilliseconds;
#else
			struct timeval tp;
			gettimeofday(&tp, NULL);
			unsigned long long current = (static_cast<unsigned long long>(tp.tv_sec) * 1000);
			result = (current + (tp.tv_usec / 1000));
#endif
			return result;
		}
        
        virtual String toString() const {
            return String("Currently at: ") + String::fromLong(StoppableClock::GetCurrentTimeStamp()) +
                    String(" | Started at: ") + String::fromLong(this->startTimeStamp) 
                    + String(" | Duration: ") + String::fromLong(this->getDuration()) 
                    + String("ms");
        }
};

#endif
