#ifndef __DESCRIBED_TYPE__
#define __DESCRIBED_TYPE__

#include "StringWrapper.h"
#include <iostream>

template<class _T> class Described {
    private:
        _T value;
        String description;
    public:
        Described() {
        	this->value = _T();
        	this->description = String();
        }

        Described(const _T& val, const char* desc) : Described(val, String(desc)) {
        }
        
        Described(const _T& val, const String& desc) {
            this->setValue(val);
            this->setDescription(desc);
        }

		virtual ~Described() {
		}
        
        __inline Described& operator=(const _T& v) {
            this->setValue(v);
            return (*this);
        }
        
        __inline Described& operator=(const Described<_T>& other) {
            this->setValue(other.value);
            this->setDescription(other.description);
            return (*this);
        }
        
        explicit operator _T() const {
        	return this->value;
        }

        __inline void setValue(const _T& v) {
            this->value = v;
        }
        
        __inline const _T& getValue() const {
            return this->value;
        }
        
        __inline void setDescription(const String& s) {
            this->description = s;
        }
        
        __inline const String& getDescription() const {
            return this->description;
        }
        
        friend std::ostream& operator<<(std::ostream& o, const Described<_T>& t) {
            o << t.getDescription().toConstChar() << ": " << t.getValue();
            return o;
        }
};

#endif //__DESCRIBED_TYPE__
