
#ifndef __LOGGER__
#define __LOGGER__

#include "StringWrapper.h"
#include "DescribedType.h"

class LogLevel {
private:
    unsigned char levelValue;
public:
    LogLevel() {
    	this->levelValue = 0;
    }
    LogLevel(unsigned char lvl) {
        this->levelValue = lvl;
    }
    
    __inline LogLevel& operator=(const LogLevel& c) {
        this->levelValue = c.levelValue;
        return (*this);
    }
    
    __inline bool operator<=(const LogLevel& l) const {
        return this->levelValue <= l.levelValue;
    }
    __inline bool operator>=(const LogLevel& l) const {
        return this->levelValue >= l.levelValue;
	}
	static Described<LogLevel*> GLOBAL_OVERRIDE;
	const static Described<LogLevel*> DEBUG;
	const static Described<LogLevel*> INFO;
	const static Described<LogLevel*> WARN;
	const static Described<LogLevel*> _ERROR;
};

class Logger {
    private:
        String classNameToLog;
		Described<LogLevel*> logLevel;

		class FileWriter* outLog;
        
        const String& getClassLogName() const;
		void internalLog(const Described<LogLevel*>& levelToCheckFor, const String& msg) const;
    public:
        Logger();
        Logger(const class BasicObject& o);
        Logger(const class BasicObject* o) : Logger(*o) {};
        ~Logger();
        
        Logger& operator=(const Logger& other) {
            this->classNameToLog = other.classNameToLog;
            return (*this);
        }

		void attachFileWriter(FileWriter *fw) {
			this->outLog = fw;
		}
        
		const Described<LogLevel*>& getLogLevel() const;
		void setLogLevel(const Described<LogLevel*>& l);
        
        void debug(const String& msg) const;
        __inline void debug(const char* msg) const {
            this->debug(String(msg));
        }
        __inline static void Debug(const String& msg) {
            Logger().debug(msg);
        }
        __inline static void Debug(const char* msg) {
            Logger().debug(msg);
        }
        
        void info(const String& msg) const;
        void info(const char* msg) const {
            this->info(String(msg));
        }
        __inline static void Info(const String& msg) {
            Logger().info(msg);
        }
        __inline static void Info(const char* msg) {
            Logger().info(msg);
        }
        
        void warn(const String& msg) const;
        void warn(const char* msg) const {
            this->warn(String(msg));
        }
        __inline static void Warn(const String& msg) {
            Logger().warn(msg);
        }
        __inline static void Warn(const char* msg) {
            Logger().warn(msg);
        }
        
        void error(const String& msg) const;
        void error(const char* msg) const {
            this->error(String(msg));
        }
        __inline static void Error(const String& msg) {
            Logger().error(msg);
        } 
        __inline static void Error(const char* msg) {
            Logger().error(msg);
        }
};

#endif
