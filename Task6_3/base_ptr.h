#ifndef BASE_PTR_H
#define BASE_PTR_H

// base class to inherit
template<typename T>
class TBasePtr {
protected:
    T* _ptr{nullptr};
public:
    T* operator->() const {
        return _ptr;
    }
    T& operator*() const {
        if (_ptr != nullptr) {
            return *_ptr;
        }
    }
    bool operator==(const TBasePtr<T>& b) const {
        return _ptr == b._ptr;
    }
    bool operator!=(const TBasePtr<T>& b) const {
        return _ptr != b._ptr;
    }
    operator bool() const {
        return _ptr != nullptr;
    }
};

#endif