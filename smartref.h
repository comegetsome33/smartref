#pragma once

template <class T>
struct DefaultRefCounterGetter
{
    static int& Get(T*ptr) { return ptr->refCounter; }
};

template <class T, class RefCounterGetter = DefaultRefCounterGetter<T>>
class Ref
{
    T* ptr;
public:
    template <class U>
    using AltRef = Ref<U, RefCounterGetter>;
    
    template <class U>
    friend class AltRef;
    
    Ref() : ptr { nullptr } { }
    
    Ref(T* ptr) : ptr { ptr } { if(ptr)RefCounterGetter::Get(ptr)++; }
    
    template <class U>
    Ref(U* ptr) : ptr { ptr } { if(ptr)RefCounterGetter::Get(ptr)++; }
    
    Ref(const Ref<T, RefCounterGetter>& other) : ptr { other.ptr } { RefCounterGetter::Get(ptr)++; }
    
    template <class U>
    Ref(const Ref<U, RefCounterGetter>& other) : ptr { other.ptr } { RefCounterGetter::Get(ptr)++; }
    
    Ref(Ref<T, RefCounterGetter>&& other) : ptr { other.ptr } { other.ptr = nullptr; }
    
    template <class U>
    Ref(Ref<U, RefCounterGetter>&& other) : ptr { other.ptr } { other.ptr = nullptr; }
    
    Ref<T, RefCounterGetter>& operator=(const Ref<T, RefCounterGetter>& other)
    {
        if(ptr && !--RefCounterGetter::Get(ptr)) delete ptr;
        ptr = other.ptr;
        if(ptr) RefCounterGetter::Get(ptr)++;
        return *this;
    }
    
    template <class U>
    Ref<U, RefCounterGetter>& operator=(const Ref<U, RefCounterGetter>& other)
    {
        if(ptr && !--RefCounterGetter::Get(ptr)) delete ptr;
        ptr = other.ptr;
        if(ptr) RefCounterGetter::Get(ptr)++;
        return *this;
    }
    
    template <class U>
    Ref<T, RefCounterGetter>& operator=(U* value)
    {
        if(ptr == value) return *this;
        if(ptr && !--RefCounterGetter::Get(ptr)) delete ptr;
        ptr = value;
        if(ptr) RefCounterGetter::Get(ptr)++;
        return *this;
    }
    
    ~Ref() { if(ptr && !--RefCounterGetter::Get(ptr)) delete ptr; }
    
    template <class U, class ... Args>
    static Ref<T, RefCounterGetter> Construct(Args&& ... args)
    { return new T(std::forward<Args>(args)...); }
    
    T* operator ->() const { return ptr; }
    
    T& operator *() const { return *ptr; }
    
    operator bool() const { return ptr; }
    
    template <class U>
    bool operator==(const AltRef<U>& other) const { return ptr == other.ptr; }
    
    template <class U>
    bool operator==(U* other) const { return ptr == other; }
    
    template <class U>
    bool operator!=(const AltRef<T>& other) const { return ptr != other.ptr; }
    
    template <class U>
    bool operator!=(U* other) const { return ptr != other; }
    
    template <class U>
    Ref<U, RefCounterGetter> DynamicCast() { return dynamic_cast<U>(ptr); }
};