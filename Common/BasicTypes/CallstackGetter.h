#ifndef __CALLSTACK_GETTER__
#define __CALLSTACK_GETTER__

#include "BasicObject.h"
#include "SharedArrayPointer.h"
#include "File.h"

#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#ifdef _MSC_VER
#pragma comment(lib, "dbghelp.lib")
#else

#endif //_MSC_VER
#else
#include <execinfo.h>
#include <cxxabi.h>
#endif

class Stacktrace: public BasicObject {
private:
	String functionName;
	String line;
	String adress;
public:
	Stacktrace() {
		this->logger = Logger(this);
		this->functionName = this->line = this->adress = String();
	}
	explicit Stacktrace(const Stacktrace& st) : Stacktrace() {
		(*this) = st;
	}
	Stacktrace(void *ptr) : Stacktrace() {
		DWORD64 x = 0;
		char buffer[0x100 + sizeof(SYMBOL_INFO)] = { 0x00 };
		SYMBOL_INFO *symbol = (SYMBOL_INFO *) buffer;
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->Address = 0;

		symbol->Address = (DWORD_PTR) (ptr);
		if (!SymFromAddr(GetCurrentProcess(), symbol->Address, &x, symbol)) {
			this->functionName = String("(Error: ")
					+ String::fromInt(GetLastError())
					+ String(") Unknown function");
		} else {
			IMAGEHLP_LINE64 img;
			DWORD y = 0;
			this->functionName = String(symbol->Name);
			if (SymGetLineFromAddr64(GetCurrentProcess(), symbol->Address, &y,
					&img)) {
				this->line = String::fromInt(img.LineNumber);
			}
			this->adress = String::fromHex(ptr);
		}
	}
	explicit Stacktrace(Stacktrace&& st) {
		(*this) = st;
	}

	Stacktrace& operator=(const Stacktrace& st) {
		this->functionName = st.functionName;
		this->line = st.line;
		this->adress = st.adress;
		return (*this);
	}

	Stacktrace& operator=(Stacktrace&& other) {
		this->adress = std::move(other.adress);
		this->functionName = std::move(other.functionName);
		this->line = std::move(other.line);
		return (*this);
	}
	virtual ~Stacktrace() {
	}

	__inline String getFunctionName() const {
		return this->functionName;
	}

	__inline String getLine() const {
		return this->line;
	}

	__inline String getFunctionAdress() const {
		return this->adress;
	}

	String toString() const {
		return this->getFunctionName() + String(":") + this->getLine() + String("(@") + this->getFunctionAdress() + String(")");
	}
};

class Callstack {
private:
	static bool SYM_INITIALIZED;

#ifdef _WIN32
	static SharedArrayPtr<Stacktrace> GetStacktraceWIN() {
		if (!SYM_INITIALIZED) {
			SymInitialize(GetCurrentProcess(),
					"https://msdl.microsoft.com/download/symbols", FALSE);
			SymSetOptions(
					SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_ALLOW_ABSOLUTE_SYMBOLS | SYMOPT_UNDNAME | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_DEBUG);
			SymRefreshModuleList(GetCurrentProcess());
			SYM_INITIALIZED = true;
		}

		void *stack[100] = { NULL };
		unsigned short frameAmount = CaptureStackBackTrace(1, 100, stack, NULL);

		SharedArrayPtr<Stacktrace> result(new Stacktrace[frameAmount], frameAmount);
		for (unsigned short i = 0; i < frameAmount; i++) {
			result.at(i) = Stacktrace(stack[i]);
		}
		return result;
	}
#endif
public:
	static SharedArrayPtr<Stacktrace> GetStacktrace() {
#ifdef _WIN32
		return GetStacktraceWIN();
#else
		return SharedArrayPtr<String>();
#endif
	}

};

#endif //__CALLSTACK_GETTER__
