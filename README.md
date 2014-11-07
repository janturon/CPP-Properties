CPP-Properties
==============
By including the property.h file C++ will support properties. To understand the syntax, see the example first:

    struct Test {
      typedef property<Test,int> Int;
      typedef property<Test,std::string> String;
      typedef property<Test,int*> IntP;
      Int P1, P2[6];
      String P3;
      IntP P4;
      Test() : P1(this), P3(this, "Hello"), P4(this) {
        for(int i=0; i<6; ++i) P2[i].create(this,i);
        // add some functionality to properties
      }
    };

Properties must know its context, which is the object of struct Test here. Therefore they should be bound with constructor call like P1, P3 and P4. P2 is an array of properties, so it is created by default constructor and the context is provided in the `create()` method.
Every property holds its own `.value` (auto-implemented, accessible by i.e. P1.value) which can be initialized as the second argument of the constructor (see P3) or the second argument of the create method (see P2). This is useful to types without default constructor. If the value hasn't been provided, the default type constructor is natively called, which doesn't have to define the value! In the example, P1 and P4 are undefined. They can be lazy initialized by calling the `.init()` chain method: its argument is invoked at the attempt to read undefined value. It can be direct value or method. The method can be handy to implement the NullObject pattern:

    struct Test {
      ...
      int nullObject;

      void lazyInit(int* val) {
        P4.value = &nullObject;
        // do something
      }

      Test() : ... {
        nullObject = 42;
        P1.init(0);
        P4.init(&Test::lazyInit);
      }
    };

To check if property is defined at some time, call the `.valid()` method:

    if(P1.valid()) // do something

The properties overloaded the assign and the type operator

    T operator=(const T& val) {...} // generic assign, applies to property = typeValue
    operator T() {...} // generic type, applies to typeTarget = property

The *assign operator* is bound with functor operator and the *type operator* is bound with functor without params, so you can call i.e

    Test t;
    t.P3 = "Hello"; // assigns the copy
    t.P3("world"); // assigns the reference
    string s = t.P3; // reads
    puts(t.P3().c_str()); // reads and prints "world"
  
Where *type* means the type of the property value (i.e. string at P2). Auto-implementation reads and writes their values directly. To make some property read-only, use i.e.:

    P1.write(false);
    P3.read(true).write(false);

By default read and write are true. If forbidden read is performed, 1 is thrown, if forbidden write is performed, 2 is thrown. This behavior can be substitued by method calls, i.e.

    struct Test {
      ...
      int*& getP3() {
        // do something
        return P2.value;
      }

      Test() : ... {
        P3.get(&Test::getP2);
      }
    };

The `.get()` calls overwrites eventual previous `.read()` settings and vice versa. Note that the handler always returns reference type, even reference to the pointer. There can be a general handler: the first argument can be reference to a property, in which case it is filled with the property that called it, i.e.

    struct Test {
      typedef property<Test,int> Int;
      Int P1,P2;
      ...
      int& getP(Int& src) {
        // do something
        return src.value;
      }

      Test() : ... {
        P1.get(&Test::getP);
        P2.get(&Test::getP);
      }
    };

Of course there is also the `.set()` chain method, including the general version:

    struct Test {
      typedef property<Test,int> Int;
      Int P1,P2;
      ...
      int& getP(Int& src) {
        // do something
        return src.value;
      }

      void setP(Int& src, int val) {
        // do something
        src.value = val;
      }

      void setP1(int val) {
        // do something
        P1.value = val;
      }

      Test() : ... {
        P1.get(&Test::getP).set(&Test::setP1);
        P2.set(&Test::setP).get(&Test::getP);
      }
    };

Note that reading and writing the value directly doesn't call eventual getter or setter:

    Test t;
    t.P1 = something; // calls setter, returns somethings value
    t.P1(something); // calls setter, no overhead
    t.P1.value = something; // direct write
    something = t.P1; // calls getter
    something = t.P1(); // calls getter and explicit cast to value type
    something = t.P1.value; // direct read

Finally, every property contains `const void* meta` pointer, so the client code can associate any additional data to every property (this is useful i.e. for arrays: the property can hold its index). There are no helping methods, just the pointer, so the type and value control is up to the client code:

    t.P1.meta = "reminder";  // write
    puts((const char*)t.P1.meta);  // read

    int n = 42;
    t.P1.meta = &n; // write
    printf("%d",*(int*)t.P1.meta); // read

The context doesn't need to be the class in which the property is defined, so "external properties" can be implemented. If you have any suggestions or bug report, mail me at janturon@email.cz. Enjoy the code on the terms of GNU-GPL licence.
