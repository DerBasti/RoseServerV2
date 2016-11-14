#include "Logger.h"
#include "BasicObject.h"
#include "TimeUnit.h"
#include "FileWriter.h"

const Described<LogLevel*> LogLevel::DEBUG = Described<LogLevel*>(new LogLevel(0x01), "DEBUG");
const Described<LogLevel*> LogLevel::INFO = Described<LogLevel*>(new LogLevel(0x02), "INFO");
const Described<LogLevel*> LogLevel::WARN = Described<LogLevel*>(new LogLevel(0x03), "WARN");
const Described<LogLevel*> LogLevel::_ERROR = Described<LogLevel*>(new LogLevel(0x04), "ERROR");
Described<LogLevel*> LogLevel::GLOBAL_OVERRIDE = LogLevel::_ERROR;

Logger::Logger() {
    this->classNameToLog = String("GLOBAL");
    this->logLevel = LogLevel::INFO;
	this->outLog = nullptr;
}

Logger::Logger(const BasicObject& o) {
    this->classNameToLog = o.getClassName();
	this->logLevel = LogLevel::INFO;
	this->outLog = nullptr;
}

Logger::~Logger() {
    
}

const Described<LogLevel*>& Logger::getLogLevel() const {
    return this->logLevel;
}

void Logger::setLogLevel(const Described<LogLevel*>& l) {
    this->logLevel = l;
}

const String& Logger::getClassLogName() const {
    return this->classNameToLog;
}

void Logger::internalLog(const Described<LogLevel*>& levelToCheckFor, const String& msg) const {
	LogLevel& lvlCheck = *levelToCheckFor.getValue();
	LogLevel& currentLvl = *this->getLogLevel().getValue();
	if (lvlCheck >= currentLvl || lvlCheck >= *(LogLevel::GLOBAL_OVERRIDE.getValue())) {
		String output = String(" [") + levelToCheckFor.getDescription() + String("] ") + this->getClassLogName() 
			+ String(" - ") + String(msg);
		std::cout << TimeUnits::GetCurrentDayTimeWithMillis() << output << "\n";

		if (this->outLog) {
			outLog->reopen();
			outLog->writeLine(output);
			outLog->close();
		}
    }
}

void Logger::debug(const String& msg) const {
    this->internalLog(LogLevel::DEBUG, msg);
}

void Logger::info(const String& msg) const {
    this->internalLog(LogLevel::INFO, msg);
}

void Logger::warn(const String& msg) const {
    this->internalLog(LogLevel::WARN, msg);
}

void Logger::error(const String& msg) const{
    this->internalLog(LogLevel::_ERROR, msg);
}
