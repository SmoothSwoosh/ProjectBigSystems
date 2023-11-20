#ifndef INTRUSIVE_PTR_H
#define INTRUSIVE_PTR_H

#include "base_ptr.h"
#include "ref_counter.h"
#include "utility"

// main class (implementation of intrusive_ptr)
template<typename T>
class TIntrusivePtr : public TBasePtr<T> {
public:
    unsigned int UseCount() const {
        return (this->_ptr ? this->_ptr->RefCount() : 0);
    }

    TIntrusivePtr() {
        this->_ptr = nullptr;
    }

    TIntrusivePtr(const TIntrusivePtr& ptr) {
        this->_ptr = ptr.Get();
        if (this->_ptr) {
            this->_ptr->IncRef();
        }
    }

    TIntrusivePtr& operator=(const TIntrusivePtr& b) {
        Release();
        this->_ptr = b.Get();
        if (this->_ptr) {
            this->_ptr->IncRef();
        }
        return *this;
    }

    TIntrusivePtr(TIntrusivePtr&& ptr) {
        this->_ptr = ptr.Get();
        ptr._ptr = nullptr;
    }

    TIntrusivePtr& operator=(TIntrusivePtr&& b) {
        Release();
        this->_ptr = b.Get();
        b._ptr = nullptr;
        return *this;
    }

    TIntrusivePtr(T* ptr) {
        this->_ptr = ptr;
        if (this->_ptr) {
            this->_ptr->IncRef();
        }
    }

    T* Get() const {
        return this->_ptr;
    }

    void Release() {
        if (this->_ptr) {
            this->_ptr->DecRef();
            if (!this->_ptr->RefCount()) {
                delete this->_ptr;
            }
            this->_ptr = nullptr;
        }
    }

    void Reset() {
        Release();
    }

    void Reset(const TIntrusivePtr& ptr) {
        Release();
        this->_ptr = ptr.Get();
        if (this->_ptr) {
            this->_ptr->IncRef();
        }
    }

    void Reset(TIntrusivePtr&& ptr) {
        Release();
        this->_ptr = ptr.Get();
        ptr._ptr = nullptr;
    }
    
    void Reset(T* ptr) {
        Release();
        this->_ptr = ptr;
        if (this->_ptr) {
            this->_ptr->IncRef();
        }
    }

    ~TIntrusivePtr() {
        Reset();
    }
};

template<typename T, typename... Targs>
TIntrusivePtr<T> MakeIntrusive(Targs&&... args) {
    return TIntrusivePtr<T>(new T(std::forward<Targs>(args)...));
}

#endif