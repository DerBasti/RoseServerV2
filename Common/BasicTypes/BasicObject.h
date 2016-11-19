#ifdef _MSC_VER
#pragma once
#endif

#ifndef __BASIC_OBJECT__
#define __BASIC_OBJECT__

#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include <memory>
#include <cstdlib>
#include <functional>
#include "StringWrapper.h"
#include "TimeUnit.h"
#include "Logger.h"

typedef unsigned char byte_t;
typedef unsigned short word_t;
typedef unsigned long dword_t;
typedef unsigned long long qword_t;

class BasicObject {
	protected:
		Logger logger;

		static String determineClassName(const BasicObject *obj) {
#ifndef _MSC_VER
			int status = -1;
			const char *name = typeid(*obj).name();

			std::unique_ptr<char, void(*)(void*)> res {
				abi::__cxa_demangle(name, NULL, NULL, &status),
			    std::free
			};

			String result = (status == 0) ? String((const char*)res.get()) : String(name);
			return result;
	#else
			std::string fullName = typeid(*obj).name();
			return String(fullName.substr(fullName.find(" ")+1).c_str());
	#endif
		}
	public:
		BasicObject() {
		}

		virtual ~BasicObject() {
		}

		virtual String toString() const {
			return this->getClassName();
		}

		virtual String getClassName() const {
			return BasicObject::determineClassName(this);
		}

		virtual BasicObject& operator*() const {
			return const_cast<BasicObject&>(*this);
		}

		virtual BasicObject* operator->() const {
			return const_cast<BasicObject*>(this);
		}

		Logger& getLogger() {
			return this->logger;
		}

		friend std::ostream& operator<<(std::ostream& out, const BasicObject& s) {
			String str = s.toString();
			out << str;
			return out;
		}
};

class Toggleable : public BasicObject {
	private:
		bool value;
	public:
		Toggleable() {
			this->logger = Logger(this);
			this->value = false;
		}
		virtual ~Toggleable() {
			this->value = false;
		}

		__inline operator bool() const {
			return this->value;
		}

		__inline bool get() const {
			return this->value;
		}

		void toggle() {
			this->logger.info("Toggled value to true.");
			this->value = true;
		}
		void untoggle() {
			this->logger.info("Toggled value to false.");
			this->value = false;
		}
};

template<class _T = void*> class Autoclose : public BasicObject {
private:
	std::function<void()> closer;
	_T handle;
protected:
	__inline void setHandle(_T newHandle) {
		this->handle = newHandle;
	}
public:
	Autoclose() {
		this->closer = []() {};
	}
	Autoclose(_T handle) : Autoclose() {
		this->handle = handle;
	}
	virtual ~Autoclose() {
		this->closer();
	}

	Autoclose& operator=(const _T handle) {
		this->handle = handle;
		return (*this);
	}

	void setAutoclose(std::function<void()> f) {
		this->closer = f;
	}

	operator _T() {
		return this->handle;
	}

	_T get() const {
		return this->handle;
	}
};

#endif //__BASIC_OBJECT__
