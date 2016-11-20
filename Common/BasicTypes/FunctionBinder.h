#ifndef __FUNCTION_BINDER__
#define __FUNCTION_BINDER__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>
#include <memory>
#include <algorithm>

template<class ClassToBind, class IdType, class SetFunction = void (ClassToBind::*)(const IdType t), class GetFunction = IdType(ClassToBind::*)() const>
class FunctionBinder {
private:
	std::map<IdType, GetFunction> getFunctions;
	std::map<IdType, SetFunction> setFunctions;
public:
	FunctionBinder() {}
	FunctionBinder(std::initializer_list<std::pair<IdType, SetFunction>> setFunctions) {
		std::for_each(setFunctions.begin(), setFunctions.end(), [&](std::pair<IdType, SetFunction> pair) {
			this->setFunctions[pair.first] = pair.second;
		});
	}
	virtual ~FunctionBinder() {}

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

template<class IdType, class SetFunction = void(*)(const IdType t), class GetFunction = IdType(*)() const>
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
struct DataInterpreter {
	unsigned long caret;
	unsigned long length;
	_T* data;

	DataInterpreter() : DataInterpreter(nullptr, 0) {}
	DataInterpreter(_T* data, unsigned long len) {
		this->length = len;
		this->data = data;
		this->caret = 0;
	}
	virtual ~DataInterpreter() {
		this->data = nullptr;
		this->caret = this->length = 0x00;
	}
	template<class _X = _T>
	__inline _X get() {
		_X currentData = *((_X*)(this->data + this->caret));
		this->caret += sizeof(_X);
		return currentData;
	}
};

#endif