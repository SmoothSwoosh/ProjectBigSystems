#include "intrusive_ptr.h"
#include "assert.h"

class TDoc : public TRefCounter<TDoc> {
    // ...
};

// ...

int main() {
    TIntrusivePtr<TDoc> ptr = nullptr;
    ptr = MakeIntrusive<TDoc>(); // ptr.UseCount() == 1
    assert(ptr.UseCount() == 1);

    TIntrusivePtr<TDoc> ptr2 = ptr; // ptr.UseCount() == 2
    assert(ptr.UseCount() == 2);

    TIntrusivePtr<TDoc> ptr3 = MakeIntrusive<TDoc>();
    ptr3.Reset(ptr2); // ptr.UseCount() == ptr3.UseCount() == 3
    assert(ptr.UseCount() == ptr3.UseCount());
    assert(ptr.UseCount() == 3);

    ptr3.Reset(); // ptr.UseCount() == ptr3.UseCount() == 2
    assert(ptr.UseCount() == 2);

    ptr3.Reset(std::move(ptr2)); // ptr.UseCount() == ptr3.UseCount() == 2
    assert(ptr.UseCount() == ptr3.UseCount());
    assert(ptr.UseCount() == 2);

    static_assert(sizeof(TDoc*) == sizeof(ptr));
    return 0;
}