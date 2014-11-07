#pragma once

/**
 * T propery type
 * C class context
 */
template<typename C, typename T>
class property {
  enum PropType { Method, Handler, Logical };
  typedef T&(C::*MethodGet)();
  typedef T&(C::*HandlerGet)(property<C,T>&);
  typedef void(C::*MethodSet)(const T);
  typedef void(C::*HandlerSet)(property<C,T>&, const T);
  typedef void(C::*MethodInvalid)(property<C,T>&);

  PropType getType, setType;
  MethodGet Mget; MethodSet Mset;
  HandlerGet Hget; HandlerSet Hset;
  bool Lget, Lset;
  bool undefined;
  MethodInvalid initMethod;
  C* context;

public:
  property<C,T>& create(C* c) {
    undefined = true; context = c;
    Lget = true; Lset = true;
    getType = setType = Logical;
    initMethod = 0;
    return *this;
  }
  property<C,T>& create(C* c, T initVal) { value = initVal; return create(c); }
  property<C,T>& create(C* c, T* initVal) { value = *initVal; return create(c); }

  T& operator()() {
    if(undefined && initMethod!=0) ((context)->*initMethod)(*this);
    switch(getType) {
      case Method: return ((context)->*Mget)();
      case Handler: return ((context)->*Hget)(*this);
      case Logical: if(Lget) return value; else throw 1;
    }
    return value;
  }
  void operator()(const T& val) {
    undefined = false;
    switch(setType) {
      case Method: ((context)->*Mset)(val); break;
      case Handler: ((context)->*Hset)(*this,val); break;
      case Logical: if(!Lset) throw 2; else value = val; break;
    }
  }

  T value;
  property(C* c) { create(c); }
  property(C* c, T initVal) { create(c,initVal); }
  property(C* c, T* initVal) { create(c,initVal); }
  property() { }

  property<C,T>& get(MethodGet g) { getType = Method; Mget = g; return *this; }
  property<C,T>& set(MethodSet s) { setType = Method; Mset = s; return *this; }
  property<C,T>& get(HandlerGet g) { getType = Handler; Hget = g; return *this; }
  property<C,T>& set(HandlerSet s) { setType = Handler; Hset = s; return *this; }
  property<C,T>& read(bool g) { getType = Logical; Lget = g; return *this; }
  property<C,T>& write(bool s) { setType = Logical; Lset = s; return *this; }
  T operator=(const T& val) { (*this)(val); return val; }
  operator T() { return (*this)(); }

  bool valid() { return !undefined; }
  property<C,T>& init(const T& initVal) { value = initVal; return *this; }
  property<C,T>& init(MethodInvalid initVal) { initMethod = initVal; return *this; }

  const void* meta;
};