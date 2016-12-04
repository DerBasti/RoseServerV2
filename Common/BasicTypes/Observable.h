#ifndef __OBSERVABLE__
#define __OBSERVABLE__

#include "BasicObject.h"
#include <functional>
#include <vector>
#include <type_traits>

template<class _T, class = typename std::enable_if<std::is_assignable<_T&, const _T&>::value>::type> class Observable : public BasicObject {
	private:
		_T value;
		std::function<void(const _T&)> onNewValue;
		std::function<void(const _T)> onValueChange;
		bool triggerOnce;
		unsigned long triggerCount;
	public:
		Observable() {
			this->onNewValue = [](const _T&){};
			this->onValueChange = [](const _T) {};
			triggerOnce = false;
			triggerCount = 0;
		}

		Observable(const _T& val) : Observable() {
			(*this) = val;
		}
		virtual ~Observable() {
		}

		__inline Observable& operator=(const _T& val) {
			this->triggerCount++;
			if (!triggerOnce || (this->triggerCount <= 1 && triggerOnce)) {
				this->onNewValue(val);
			}
			const _T oldValue = this->value;
			this->value = val;
			if (!triggerOnce || (this->triggerCount <= 1 && triggerOnce)) {
				this->onValueChange(oldValue);
			}
			this->triggerCount--;
			return (*this);
		}

		__inline Observable& operator=(const Observable<_T>& observ) {
			(*this) = observ.value;
			return (*this);
		}

		__inline bool operator==(const _T& value) const {
			return this->getValue() == value;
		}

		operator _T() {
			return this->getValue();
		}

		__inline void setOnValueChange(std::function<void(const _T& newValue)> f) {
			this->onNewValue = f;
		}

		__inline void setOnNewValueAssigned(std::function<void(const _T previousValue)> f) {
			this->onValueChange = f;
		}
		__inline std::function<void(const _T previousValue)> getOnNewValueAssigned() const {
			return this->onValueChange;
		}

		__inline _T getValue() const {
			return this->value;
		}

		__inline _T& getMutableValue() {
			return this->value;
		}
		__inline const _T& getRefValue() const {
			return this->value;
		}
		__inline void setTriggerListenerOnce(bool flag) {
			this->triggerOnce = flag;
		}
};

#endif //__OBSERVABLE__
