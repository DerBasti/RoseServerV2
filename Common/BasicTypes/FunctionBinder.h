#ifndef __FUNCTION_BINDER__
#define __FUNCTION_BINDER__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>
#include <memory>
#include <algorithm>
#include <initializer_list>
#include <tuple>
#include <vector>

#include "StringWrapper.h"

template<class ClassToBind, class IdType, class SetFunction = void (ClassToBind::*)(const IdType t), class GetFunction = IdType(ClassToBind::*)() const>
class FunctionBinder {
private:
	std::map<IdType, GetFunction> getFunctions;
	std::map<IdType, SetFunction> setFunctions;
public:
	FunctionBinder() {}
	FunctionBinder(std::initializer_list<std::pair<IdType, GetFunction>> getFunctions) {
		std::for_each(getFunctions.begin(), getFunctions.end(), [&](std::pair<IdType, GetFunction> pair) {
			this->getFunctions[pair.first] = pair.second;
		});
	}
	FunctionBinder(std::initializer_list<std::pair<IdType, SetFunction>> setFunctions) {
		std::for_each(setFunctions.begin(), setFunctions.end(), [&](std::pair<IdType, SetFunction> pair) {
			this->setFunctions[pair.first] = pair.second;
		});
	}
	FunctionBinder(std::initializer_list<std::tuple<IdType, SetFunction, GetFunction>> tuple) {
		(*this) = tuple;
	}
	virtual ~FunctionBinder() {}

	FunctionBinder<ClassToBind, IdType, SetFunction, GetFunction>& operator=(std::initializer_list<std::tuple<IdType, SetFunction, GetFunction>> list) {
		std::for_each(list.begin(), list.end(), [&](std::tuple<IdType, SetFunction, GetFunction> pair) {
			this->setFunctions[std::get<0>(pair)] = std::get<1>(pair);
			this->getFunctions[std::get<0>(pair)] = std::get<2>(pair);
		});
		return (*this);
	}

	__inline void bindProcessingFunction(const IdType id, SetFunction function) {
		this->setFunctions[id] = function;
	}
	__inline void bindGetterFunction(const IdType id, GetFunction function) {
		this->getFunctions[id] = function;
	}

	void bindFunctions(const IdType id, GetFunction getter, SetFunction setter) {
		this->setFunctions[id] = setter;
		this->getFunctions[id] = getter;
	}

	__inline SetFunction getProcessingFunction(const IdType id) {
		return this->setFunctions[id];
	}
	__inline GetFunction getGetterFunction(const IdType id) {
		return this->getFunctions[id];
	}

	virtual __inline size_t getSizeOfProcessingFunctions() const {
		return this->setFunctions.size();
	}
	virtual __inline size_t getSizeOfGetters() const {
		return this->getFunctions.size();
	}
};

template<class IdType, class SetFunction = void(*)(const IdType t), class GetFunction = IdType(*)()>
using StaticFunctionBinder = FunctionBinder<void*, IdType, SetFunction, GetFunction>;

template<class ClassToBind>
using FunctionToIDBinder = FunctionBinder<ClassToBind, unsigned long>;

template<class _BaseType>
class InstanceCreator {
public:
	template<typename T> static std::shared_ptr<_BaseType> create() {
		return std::shared_ptr<_BaseType>(new T);
	}
	template<typename T, typename ArgType> static std::shared_ptr<_BaseType> createWithArg(const ArgType& arg) {
		return std::shared_ptr<_BaseType>(new T(arg));
	}
};

template<class _T>
class DataInterpreter {
protected:
	unsigned long caret;
	unsigned long length;
	_T* data;
	void initProtected(_T* data, unsigned long len) {
		this->length = len;
		this->data = data;
		this->caret = 0;
	}
public:
	DataInterpreter() : DataInterpreter(nullptr, 0) {}
	DataInterpreter(_T* data, unsigned long len) {
		this->initProtected(data, len);
	}
	virtual ~DataInterpreter() {
		this->data = nullptr;
		this->caret = this->length = 0x00;
	}
	template<class _X>
	_X get(unsigned long overrideLen = 0) {
		dword_t size = sizeof(_X);
		_X currentData = *((_X*)(this->data));
		this->caret += size;
		this->data += size;
		return currentData;
	}
	String getString(const unsigned long len) {
		String result = String();
		for (unsigned int i = 0; i < len; i++) {
			result += get<byte_t>();
		}
		return result;
	}
	void addToCaret(const unsigned long pos) {
		this->caret += pos;
	}
};

template<typename... Args>
class TupleDataInterpreter : DataInterpreter<const char> {
private:
	std::tuple<Args...> arguments;
	unsigned long argLength;
	std::vector<unsigned long> values;
	void readData() {}
	template<typename _ValueType, typename... Args> void readData(_ValueType val, Args&&... args) {
		this->readValue<_ValueType>();
		this->readData(args...);
	}
	template<typename _ValueType> void readValue() {
		this->values.push_back(this->get<_ValueType>(0));
	}
public:
	TupleDataInterpreter() { }
	TupleDataInterpreter(const char *data, size_t len, Args&&... args) : arguments(std::forward<Args>(args)...) {
		this->initProtected(data, len);
		this->argLength = sizeof...(args);
		this->readData(args...);
	}
	__inline unsigned long getArgumentLength() const {
		return this->argLength;
	}
	virtual ~TupleDataInterpreter() {}

	__inline unsigned long getValue(const size_t pos) const {
		return this->values[pos];
	}

	std::tuple<Args...> getArguments() const {
		return this->arguments;
	}
};

#endif