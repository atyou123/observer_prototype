#include "../util/Channel.h"
#include "../util/Serialization.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <unordered_set>
#include <vector>

// How to represent state with observer pattern
// If state is external shared state, then what are the main objects even doing?
// Why not use funcitons instead? Should objects communicate thru only notify?
// If communicate thru notify, the message passing

// Communicating thru notify multiple times could happen even with shared state
// 1. Complete message passing and encapsulation
// 2. Shared state and message passing
// 3. Can call non-update functions, state is flexible

// How to send the messages (thru pointers or abstract lists)
// Lists more generic, more encapsulation and abstraction
// List method can't pass in argument to notify function
//      Could you maybe pass args in some way, maybe external state
// List method introduces boilerplate and complexity, might not be the best
// Abstraction might not be good for readability (Too much jumping)

// Which compiler to install

// Move semantics

using namespace std;

typedef unsigned char byte;

enum Event {
    AReceived,
    BReceived,
    CReceived,
    DReceived,
};

struct Channels {
    queue<byte> ain;
    queue<byte> aout;

    queue<byte> bin;
    queue<byte> bout;

    queue<byte> cin;
    queue<byte> cout;

    queue<byte> din;
    queue<byte> dout;
};

class A;
class B;
class C;
class D;

class A {
  public:
    void ProcessExternalMessage();

    struct Internal {
        enum Type {
            CMessage,
            DMessage,
        } sender;

        size_t lo;
        size_t hi;
    };

    void ProcessInternalMessage(Internal message);

    vector<int> numbers;
    B *b;
    C *c;
    D *d;

    queue<byte> *in;
    queue<byte> *out;
};

class B {
  public:
    void ProcessExternalMessage();

    void ProcessInternalMessage(string message);

    queue<byte> *in;
    queue<byte> *out;
};

class C {
  public:
    void ProcessExternalMessage();

    A *a;
    void ProcessInternalMessage(vector<int> message);

    queue<byte> *in;
    queue<byte> *out;
};

class D {
  public:
    void ProcessExternalMessage();
    void ProcessInternalMessage(int message);

    A *a;

    queue<byte> *in;
    queue<byte> *out;
};

void A::ProcessExternalMessage() {
    // Deserialization
    size_t b_message_size = 0;
    for (size_t i = 0; i < 8; i++) {
        b_message_size |= in->front() << (7 - i) * 8;
        in->pop();
    }

    string b_message;
    for (size_t i = 0; i < b_message_size; i++) {
        b_message.push_back(in->front());
        in->pop();
    }

    int num = 0;
    for (size_t i = 0; i < 4; i++) {
        num |= in->front() << (3 - i) * 8;
        in->pop();
    }

    b->ProcessInternalMessage(b_message);

    numbers.push_back(num);
}

void A::ProcessInternalMessage(Internal message) {
    switch (message.sender) {
    case Internal::CMessage: {
        vector<int> query_result;
        for (size_t i = message.lo; i < message.hi; i++) {
            query_result.push_back(numbers[i]);
        }
        c->ProcessInternalMessage(move(query_result));
        break;
    }
    case Internal::DMessage: {
        int total = 0;
        for (int x : numbers) {
            total += x;
        }
        d->ProcessInternalMessage(total);
        break;
    }
    }
}

void B::ProcessInternalMessage(string message) {
    for (char c : message) {
        out->push(c);
    }
}

void C::ProcessExternalMessage() {
    size_t lo = 0;
    for (size_t i = 0; i < 8; i++) {
        lo |= in->front() << (7 - i) * 8;
        in->pop();
    }

    size_t hi = 0;
    for (size_t i = 0; i < 8; i++) {
        hi |= in->front() << (7 - i) * 8;
        in->pop();
    }

    A::Internal message;
    message.sender = A::Internal::CMessage;
    message.lo = lo;
    message.hi = hi;

    a->ProcessInternalMessage(message);
}

void C::ProcessInternalMessage(vector<int> message) {
    for (int x : message) {
        out->push(x);
    }
}

void D::ProcessExternalMessage() {
    in->pop();
    A::Internal message;
    message.sender = A::Internal::DMessage;
    a->ProcessInternalMessage(message);
}

void D::ProcessInternalMessage(int message) { out->push(message); }

// A gets message containing a string and a number
//      Sends B the string, appends number to array of numbers
// C can request to see a range in the number array (e.g. index 5 - 22)
// D can request to get the average of the whole number array

void print_array(byte *arr, size_t len) {
    for (size_t i = 0; i < len; i++) {
        cout << arr[i] << ' ';
    }
    cout << endl;
}

template <typename T> void print_vector(vector<T> &vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        cout << vec[i] << ' ';
    }
    cout << endl;
}

void print_queue(queue<byte> &queue) {
    for (size_t i = 0; i < queue.size(); i++) {
        printf("%d ", queue.front());
        queue.push(queue.front());
        queue.pop();
    }
    cout << endl;
}

void push_int(queue<byte> &queue, int integer) {
    size_t bytemask = 0xFF;

    for (size_t i = 0; i < 4; i++) {
        queue.push(integer & bytemask << (3 - i) * 8);
    }
}

void push_size_t(queue<byte> &queue, size_t integer) {
    size_t bytemask = 0xFF;

    for (size_t i = 0; i < 8; i++) {
        queue.push(integer & bytemask << (7 - i) * 8);
    }
}

void push_string(queue<byte> &queue, string string) {
    for (char c : string) {
        queue.push(c);
    }
}

struct Modules {
    A a;
    B b;
    C c;
    D d;
};

void loop_iteration(Event event, Modules &mods) {
    switch (event) {
    case AReceived:
        mods.a.ProcessExternalMessage();
        break;
    case BReceived:
        break;
    case CReceived:
        mods.c.ProcessExternalMessage();
        break;
    case DReceived:
        mods.d.ProcessExternalMessage();
        break;
    }
}

int main() {
    Channels channels;
    Modules mods;

    mods.a.in = &channels.ain;
    mods.a.out = &channels.aout;
    mods.b.in = &channels.bin;
    mods.b.out = &channels.bout;
    mods.c.in = &channels.cin;
    mods.c.out = &channels.cout;
    mods.d.in = &channels.din;
    mods.d.out = &channels.dout;

    mods.a.b = &mods.b;
    mods.a.c = &mods.c;
    mods.a.d = &mods.d;

    mods.c.a = &mods.a;
    mods.d.a = &mods.a;

    push_size_t(channels.ain, 5);
    push_string(channels.ain, "hello");
    push_int(channels.ain, 10);
    loop_iteration(AReceived, mods);

    push_size_t(channels.ain, 3);
    push_string(channels.ain, "abc");
    push_int(channels.ain, 30);
    loop_iteration(AReceived, mods);

    push_size_t(channels.ain, 3);
    push_string(channels.ain, "abc");
    push_int(channels.ain, 25);
    loop_iteration(AReceived, mods);

    push_size_t(channels.ain, 3);
    push_string(channels.ain, "abc");
    push_int(channels.ain, 9);
    loop_iteration(AReceived, mods);

    push_size_t(channels.cin, 1);
    push_size_t(channels.cin, 3);
    loop_iteration(CReceived, mods);

    cout << channels.cout.size() << endl;
    print_queue(channels.cout);

    push_size_t(channels.din, 3);
    loop_iteration(DReceived, mods);
    print_queue(channels.dout);

    return 0;
}
