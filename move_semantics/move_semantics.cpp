#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

/**
 * This is an explanation of move semantics.
 *
 * This is probably not of much use for the project if we don't use threads/kinda useful if we do, but I find it fun
 * to write explanations for things, and this, combined with lifetimes (not explained) can be useful in safety critical
 * embedded software in general, especailly multi threaded.
 *
 * I think my explanation for this is better than most online explanations, but here are some links to good resources.
 * Rust implements move semantics in a much simpler/better/understandable way than C++. I recommend first learning it
 * through Rust, and then looking at how C++ implements it. C++ uses lvalues and rvalues to implement move semantics,
 * which doesn't make that much sense. It's like cutting paper with a knife. It kinda makes sense because you need a
 * sharp object, but you can just use scissors instead which is a cleaner cut. Also, C++ makes move semantics opt in
 * instead of making copy semantics opt in like Rust does, and allows unsafe backwards compatibility with C using raw
 * pointers
 *
 * Rust move semantics
 * - https://doc.rust-lang.org/book/ch04-01-what-is-ownership.html
 *
 * Rust move semantics applied to threads/message passing
 * - https://doc.rust-lang.org/book/ch13-01-closures.html
 * - https://doc.rust-lang.org/book/ch15-04-rc.html
 * - https://doc.rust-lang.org/book/ch16-01-threads.html
 * - https://doc.rust-lang.org/book/ch16-02-message-passing.html
 *
 * C++ lvalues and rvalues applied to move semantics
 * - https://youtu.be/fbYknr-HPYE?si=4kcrQP2yAunue63l
 * - https://youtu.be/ehMg6zvXuMY?si=bx0jR05-RrpmpnZ3
 *
 * Correspondences between Rust and C++
 * - rust closures == lambda functions == anonymous functions == first order function values
 * - rust Rc == c++ shared_ptr
 * - In rust, all heap allocated memory only follows move semantics, effectively equivalent to unique_ptr, clone() is
 * used to explicitly deep copy heap allocated data
 *
 * EXPLANATION
 *
 * Move semantics is a system describing how values are passed around between different memory locations in different
 * functions/scopes. It is an alternative to copy semantics or aliasing/referencing semantics (I don't think aliasing
 * semantics is not a real term that people use but I think it's a good way to describe the semantics)
 *
 * Variables are copied when they are moved to a different memory location on the stack (for example when a function is
 * called and args are passed in). For stack allocated objects like ints and structs with no pointers, the object is
 * simply copied from one stack frame to another. This is called copy semantics. However for heap allocated pointers, a
 * shallow copy is done (the pointer is copied but not the memory the pointer is pointing to). This is what I call
 * aliasing/referencing semantics. This is the traditional semantics used in C, but it can be unsafe because
 *  1. If a heap allocated structure is passed to a function, who is supposed to free the memory? It's pretty obvious in
 * a simple scenario but in a large application this is not so simple
 *  2. Aliasing can lead to bugs that are potentially hard to catch. This is not a huge issue in sequential code because
 * you know that passing a pointer into a function can result in some aliasing and data being modified, but with
 * threads, you don't want to freely alias data and share memory, as a mutex would be needed to synchronize memory
 * access to shared state
 *
 * 1 is not a problem in our simple application, where most of the memory is just allocated upfront, but move semantics
 * can be useful for a multithreaded application to ensure that state is not aliased/shared when not intended to be
 * shared and only one unique instance of a pointer is active at any given time
 *
 * The way C++ implements constructors, destructors, and lvalue/rvalue references allows us to make heap-allocated data
 * follow copy or move semantics, both of which are safer than aliasing.
 *
 * Whenever a stack allocated pointer is allocated by declaring it in a function, a constructor is called. There are
 * special construcotrs called the copy and move constructors that allow you to initialize the stack allocated memory
 * using an another stack allocated object. For example assigning newptr = oldptr; uses the oldptr to initialize the
 * newptr.
 *
 * Copy semantics means heap allocated data is deep copied when passed around through calling the copy
 * constructor, essentially making it behave semantically just like a stack-allocated variable, move semantics means at
 * any given point one and only one memory location has "ownership" of a pointer. Ownership can be moved around
 * effectively shallow copying the pointer and then invalidating the old pointer (usually by setting it to null). This
 * is done in the move constructor.
 *
 * In order to call the right constructor when assigning a class to another class, overloading is used. The constructors
 * Are overloaded so that the copy constructor takes an lvalue reference, and the move constructor takes an rvalue
 * reference which can be obtained by using the std::move() function to cast a value to an rvalue refernce.
 *
 * The containers provided by the std library (unique_ptr, shared_ptr, vector, etc.) should be preferred over raw C
 * style pointers because they use the tools (constructors, destructors, lvalue/rvalue refs) to implement copy/move
 * semantics instead of allowing you to randomly alias things. As a bonus, you can program in C++ without ever having to
 * call free/delete if you use these standard containers
 *
 * Below is an example demonstrating different memory semantics
 */

// Forward declarations to stop compiler from complaining
vector<int> g(int x, int *ptr, vector<int> copied, int *arr, vector<int> &&moved);
// This works the same way as std::vector<int>
class MyIntVector {
  public:
    MyIntVector() {
        // Making an empty vector with space to fit an item pushed back
        buf = new int[1];
        len = 1;
    }

    // Copy constructor
    MyIntVector(MyIntVector &other) {
        len = other.len;
        buf = new int[len];
        memcpy(buf, other.buf, len);
    }

    // Copy assignment
    MyIntVector &operator=(MyIntVector &other) {
        len = other.len;
        buf = new int[len];
        memcpy(buf, other.buf, len);
        return *this;
    }

    // Move constructor
    MyIntVector(MyIntVector &&other) {
        len = other.len;
        buf = other.buf;
        other.buf = nullptr;
    }

    // Move assignment
    MyIntVector &operator=(MyIntVector &&other) {
        len = other.len;
        buf = other.buf;
        other.buf = nullptr;
        return *this;
    }

    ~MyIntVector() { delete[] buf; }

    // Functions implementing the actual functionality... ommitted
    void push_back(int const &val) {}

  private:
    int *buf;
    size_t len;
};

void f() {
    // stack allocated
    int x = 3; // copy semantics
    int y = 2; // alias semantics (pointing to a local variable with a scope/lifetime, not heap allocated memory)

    // heap allocated
    vector<int> copied{10, 8, 2, 5, 9, 27}; // copy semantics
    vector<int> moved{1, 2, 3};             // move semantics
    int *arr = new int[4];                  // alias semantics
    arr[0] = 5;
    arr[1] = 3;
    arr[2] = 5;
    arr[3] = 2;

    // move(ptr) === ((type &&)ptr) is a cast
    vector<int> res = g(x, &y, copied, arr, move(moved));
    // x = 3
    // y = 9

    // res      == {10, 8, 2, 5, 9, 27, 2}  changed
    // copied   == {10, 8, 2, 5, 9, 27}     not changed

    // arr == {5, 3, 5, 9}                changed
    // moved == invalid pointer, set to null

    cout << moved[3] << endl; // undefined behavior (probably segfault)
}

vector<int> g(int x, int *ptr, vector<int> copied, int *arr, vector<int> &&moved) {
    x = 4;
    *ptr = 9;

    copied.push_back(2);
    moved.push_back(3);
    arr[3] = 9;

    return copied;
}

/*
 * The code is desugared below into a form of high-level assembly to make it clear and explicit what is happening. It
 * can help with understanding of lvalues, rvalues, initialization
 * 
 * Types: Every value has a type, here is a basic type system
 * - All C/C++ types are types (e.g. int, string, <type> *) except for lvalue/rvalue references (<type> &) and (<type> &&)
 * - if <type> is a type, then <type> lvalue and <type> rvalue are also types
 * - if <a> is a type and <b> is a type, then <a> -> <b> is the type of a function taking input <a> and returning <b>
 *
 * Operations
 * <name> -- alloc_stack(<size>)
 *      Allocates memory on the stack by moving the stack pointer of <size> bytes. <name> is now an lvalue that "points"
 *      to this location
 * <name> -- alloc_heap(<size>)
 *      Same as alloc_stack except this is on the heap. <name> is an lvalue, works similarly to a pointer but not the
 *      same as a pointer
 * ^<lvalue>
 *      "dereferencing" the lvalue to get what is written in memory at the lvalue's location, this expression becomes a
 *      concrete rvalue <lvalue> <- <rvalue> assign the rvalue to the lvalue's location in memory 
 *
 * <function>(<args>)
 *      Calls the function, the function returns. sizeof() is considered a function, same with & <function>(<args>) A
 *      function call
 */
// Function f
{
    x-- alloc_stack(sizeof(int));
    y-- alloc_stack(sizeof(int));

    copied-- alloc_stack(sizeof(MyIntVector));
    copied < -MyIntVector();
    moved-- alloc_stack(sizeof(MyIntVector));
    moved < -MyIntVector();

    heap_lvalue-- alloc_heap(sizeof(int) * 4);
    arr < -alloc_stack(sizeof(int *));
    arr < -&heap_lvalue

               res-- alloc_stack(sizeof(MyIntVector));

    return_address-- alloc_stack(sizeof(code *));
    jump to &g
}

// Function g
{
    x-- alloc_stack(sizeof(int));
    y-- alloc_stack(sizeof(int));

    jump to ^ return_address // go back to caller function
}

// The same as above, except all constructors and destructors and copying and variables are explicit
void explicitized() {
    int f_x = 3;
    int f_y = 2;

    MyIntVector f_copied = MyIntVector();
    // filling up copied
    MyIntVector f_moved = MyIntVector();
    // filling up moved
    int *f_arr = new int[4];
    // ... filling up arr

    MyIntVector f_res; // Pretend like the constructor isn't called here and things aren't initialized
    // the scope of g
    {
        int g_x = f_x;
        int *g_y = &f_y;
        // Calls the copy assignment operator because f_copied has type MyIntVector & (lvalue ref)
        // memcpy called, can be inefficient
        MyIntVector g_copied = MyIntVector(f_copied);

        // Call sthe move assignment operator because (MyIntVector &&)f_moved has type MyIntVector && (rvalue ref)
        // f_moved assigned to null
        MyIntVector g_moved = MyIntVector((MyIntVector &&) f_moved);

        int *g_arr = f_arr;

        g_x = 4;
        *g_y = 9;

        g_copied.push_back(2);
        g_moved.push_back(3);
        g_arr[3] = 9;

        // Call copy assignment operator
        f_res = g_copied;

        // buf freed for both
        g_copied.~MyIntVector();
        g_moved.~MyIntVector();
    }

    // f_copied.buf is freed
    f_copied.~MyIntVector();

    // f_moved.buf == nullptr, freeing does nothing
    f_moved.~MyIntVector();

    // f_arr is leaked
}

/**
 * Unlike std::vector, which allows for either copy semantics (implicitly), and move semantics (opt in with move()
 * function), unique_ptr blocks you from copying and you must move things around
 *
 * Rust, which does it the right way, makes unique_ptr semantics the default but allows you to opt into copying
 * using clone()
 */
class MyIntUniquePtr {
  public:
    MyIntUniquePtr(int x) {
        ptr = new int;
        *ptr = x;
    }

    // This is not needed because copy constructor/assignment are automatically deleted when move constructor is defined
    // put here anyways to be explicit
    MyIntUniquePtr(MyIntUniquePtr &other) = delete;

    MyIntUniquePtr(MyIntUniquePtr &&other) {
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    // Move assignment is automatically defined based on move constructor

    ~MyIntUniquePtr() { delete ptr; }

  private:
    int *ptr;
};

class MyIntSharedPtr {
  public:
    MyIntSharedPtr(int x) {
        ptr = new int;
        *ptr = x;
    }

    MyIntSharedPtr(MyIntSharedPtr &other) {
        refcount++;
        ptr = other.ptr;
    }

    MyIntSharedPtr(MyIntSharedPtr &&other) {
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    ~MyIntSharedPtr() {
        // Notice the pre-increment
        size_t res = --refcount;
        if (res == 0) {
            delete ptr;
        }
    }

  private:
    // No mutex around the shared ptr, multiple thread accesses can lead to data race
    int *ptr;

    // The pointer is safe to share between threads since refcount is atomic
    atomic<size_t> refcount;
};

int main() {}