
#ifndef __SHARED_ARRAY_PTR__
#define __SHARED_ARRAY_PTR__

#include <algorithm>
#include <vector>
#include "BasicObject.h"
#include "Logger.h"
#include "QuickMakros.h"

template<class _T> class SharedArrayPtr : public BasicObject {
    protected:
        _T *arrayPtr;
        unsigned long arraySize;
        
        SharedArrayPtr<_T> *base;
        std::vector<void*> refs;
        
        void init() {
            this->arrayPtr = NULL;
            this->base = NULL;
            this->arraySize = 0;
            this->logger = Logger(this);
        }
        
        void addAssignment(void *ptr) const {
            if(this != ptr) {
                this->base->refs.push_back(ptr);
            } else {
                this->logger.debug("Ignoring assignment to avoid cyclic dependency (other == this)!");
            }
        }
        
        void removeAssignment(void *ptr) const {
            for(unsigned int i=0;i<this->base->refs.size();i++) {
                if(this->base->refs[i] == ptr) {
                    this->base->refs.erase(this->base->refs.begin()+i);
                    break;
                }
            }
        }
        
        void copyRefsFrom(SharedArrayPtr<_T>* r) const {
            if(this->base != r->base) {
                this->base->refs.clear();
                std::for_each(r->base->refs.begin(), r->base->refs.end(), [&](void* p) { this->base->refs.push_back(p); });
            }
        }
        void shiftBaseToNextChild() {
            if(this->refs.size() > 0 && this->base == this) {
                this->logger.debug("Shifting base pointer to next possible child.");
                SharedArrayPtr<_T> *newBase = (SharedArrayPtr<_T>*)this->refs.at(0);

                //Remove the first entry as it isn't valid anymore
                this->refs.erase(this->refs.begin());
                newBase->base = newBase;
                newBase->copyRefsFrom(this);

                std::for_each(this->refs.begin(), this->refs.end(), [&](void *p) {
                    SharedArrayPtr<_T> *children = reinterpret_cast<SharedArrayPtr<_T>*>(p);
                    children->base = newBase;
                } );
                //Seen as we aren't the base anymore, point towards the actual base
                this->base = newBase;
            }
        }
        
        void deleteIfSoleSurviver() {
            if(this->refs.size() == 0 && this->base != NULL) {
                //In case we have a valid array and no refs, 
                //see whether we're the root node or just a leaf
                if(this->base == this) {
                    this->logger.debug("Last remaining survivor. Deleting data...");
                    //Parent node with no children left
                    delete [] this->arrayPtr;
                    this->arrayPtr = NULL;
                } else {
                    //Child node
                    this->base->removeAssignment(this);
                }
            }
        }
    public:
        FORWARD_ITERATOR_CREATOR(iterator, _T);
        REVERSE_ITERATOR_CREATOR(reverse_iterator, _T);
        
        SharedArrayPtr() {
            this->arrayPtr = NULL;
            this->base = NULL;
            this->arraySize = 0;
            this->logger = Logger(this);
        }
        explicit SharedArrayPtr(_T *data, const unsigned int size) {
            this->arrayPtr = data;
            this->arraySize = size;
            
            this->base = this;
            this->logger = Logger(this);
        }
        SharedArrayPtr(const SharedArrayPtr<_T>& p) : SharedArrayPtr<_T>() {
            this->operator=(p);
		}
        SharedArrayPtr& operator=(const SharedArrayPtr<_T>& p) {
            if(this->base != p.base) {
                this->shiftBaseToNextChild();
                this->deleteIfSoleSurviver();

                this->logger.debug("Applying new base pointer from other SharedArrayPtr.");
                this->base = p.base;
                this->arrayPtr = p.arrayPtr;
                this->arraySize = p.arraySize;

                p.addAssignment(this);
            }
            return (*this);
        }
        
        virtual ~SharedArrayPtr() {
            //We are the 'base' pointer and are about to die; hand it over!
            if(this->base) {
				this->logger.debug(String("Base currently holds: ") + String::fromLong(this->base->refs.size()) + String(" additional reference(s)."));
                this->shiftBaseToNextChild();
                this->deleteIfSoleSurviver();
            } else {
                this->logger.debug("Node has an invalid state: invalid base pointer. Nothing can be deleted.");
            }
            this->arrayPtr = NULL;
            this->base = NULL;
            this->arraySize = 0;
        }
        
        iterator begin() const {
            return iterator(this->arrayPtr);
        }
        
        iterator end() const {
            return iterator(&this->arrayPtr[this->getSize()]);
        }
        
        reverse_iterator reverse_begin() const {
            return reverse_iterator(this->arrayPtr + (this->getSize()-1));
        }
        
        reverse_iterator reverse_end() const {
            return reverse_iterator(this->arrayPtr-1);
        }
        
        operator SharedArrayPtr<_T>*() const {
            return this;
        }
        
        operator _T*() const {
            return this->arrayPtr;
        }
        
        __inline _T& operator[](const size_t pos) const {
            if(pos < 0 || pos >= this->getSize()) {
                this->logger.error("Invalid position request in operator[].");
                throw;
            }
            return this->arrayPtr[pos];
        }
        operator bool() const {
            return this->isValid();
        }
        
        __inline bool isValid() const {
            return this->arrayPtr != NULL && this->arraySize > 0;
        }
        
        __inline _T* get() const {
            return this->arrayPtr;
        }
        
        __inline _T& at(const size_t pos) const {
            return this->operator[](pos);
        }
        
        __inline unsigned long getSize() const {
            return this->arraySize;
        }
        
        __inline size_t getReferencesAmount() const {
            return (this->base != NULL ? this->base->refs.size() : 0);
        }
        
        std::vector<SharedArrayPtr<_T>*> getReferencesFromBase() const {
            std::vector<SharedArrayPtr<_T>*> res;
            std::for_each(begin(), end(), [&](void* p) {
                res.push_back(reinterpret_cast<SharedArrayPtr<_T>*>(p));
            });
            return res;
        }
};

#endif //__SHARED_ARRAY_PTR__
