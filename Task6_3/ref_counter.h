#ifndef REF_COUNTER_H
#define REF_COUNTER_H

// simple class to count references
template<typename T>
class TRefCounter {
    unsigned int _refs{0};
public:
    void IncRef() {
        ++_refs;
    }
    void DecRef() {
        if (_refs > 0) {
            --_refs;
        }
    }
    unsigned int RefCount() const {
        return _refs;
    }
};

#endif