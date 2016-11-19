#ifndef __QUICK_MAKROS__
#define __QUICK_MAKROS__

#include <iterator>

#ifndef FORWARD_ITERATOR_CREATOR
#define FORWARD_ITERATOR_CREATOR(className, dataClass) class className : public std::iterator<std::forward_iterator_tag, dataClass*> {\
            private: \
                dataClass* data; \
            public: \
                className(dataClass* d) { \
                    data = d; \
                } \
                className(const className& it) : className(it.data) { \
                     \
                } \
				className& operator+=(int val) { \
					data += val;\
					return (*this);\
				}\
				className operator+(int val) { \
					iterator copy(*this);\
					copy += val;\
					return copy;\
				}\
                className& operator++() {++data; return *this;} \
                className operator++(int) {className tmp(*this); operator++(); return tmp;} \
                bool operator==(const className& rhs) {return data==rhs.data;} \
                bool operator!=(const className& rhs) {return data!=rhs.data;} \
                dataClass& operator*() {return *data;} \
        }
#endif

#ifndef REVERSE_ITERATOR_CREATOR
#define REVERSE_ITERATOR_CREATOR(className, dataClass) class className : public std::iterator<std::forward_iterator_tag, dataClass*> {\
            private: \
                dataClass* data; \
            public: \
                className(dataClass* d) { \
                    data = d; \
                } \
                className(const className& it) : className(it.data) { \
                     \
                } \
                className& operator++() {--data; return *this;} \
                className operator++(int) {className tmp(*this); operator++(); return tmp;} \
                bool operator==(const className& rhs) {return data==rhs.data;} \
                bool operator!=(const className& rhs) {return data!=rhs.data;} \
                dataClass& operator*() {return *data;} \
        }
#endif

#endif //__QUICK_MAKROS__