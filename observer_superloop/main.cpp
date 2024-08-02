#include "../util/Channel.h"
#include "types.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <thread>
#include <unordered_set>
#include <vector>
#include <format>

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

class Unit {};

template <typename T> class Consumer {
  public:
    virtual void Update(T val) = 0;
};

template <typename T> class Producer {
  public:
    virtual void Subscribe(Consumer<T> *consumer) = 0;
};

class A;
class B;
class C;
class D;

class A : Producer<CopyBlocked<string>>, Producer<CopyBlocked<vector<int>>, Producer<int>, Consumer<CInboundMessage>, Consumer<Unit> {
  public:
    void ProcessExternalMessage(AInboundMessage msg) {}
    void Subscribe(Consumer<string> *consumer);
    void Update(CInboundMessage msg);
    void Update(Unit msg);

  private:
    void Notify(string msg) {
        for (Consumer<string> *list : listeners) {
            list->Update(msg);
        }
    }
    vector<int> nums;
    int sum;
    vector<Consumer<string> *> listeners;
};

class B : Consumer<string> {
  public:
    void Update(string msg);

  private:
    Send<u8> send;
};

class C : Producer<CInboundMessage>, Consumer<vector<int>> {
  public:
    void ProcessExternalMessage(CInboundMessage msg);
    void Subscribe(Consumer<CInboundMessage> *consumer);
    void Update(vector<int> msg);

  private:
    vector<Consumer<CInboundMessage>> listeners;
    Send<u8> send;
};

class D : Producer<Unit>, Consumer<int> {
  public:
    void ProcessExternalMessage();
    void Subscribe(Consumer<Unit> *consumer);
    void Update(int msg);

  private:
    vector<Consumer<Unit>> listeners;
    Send<u8> send;
};

class E : Consumer<string> {
  public:
    void Update(string msg) {
        msg.push_back(' ');
        msg.append(msg);
        msg.push_back(' ');
        msg.append(msg);

        send << msg;
    }

  private:
    Send<u8> send;
};

typedef Send<u8> ExternalSend;
typedef Recv<u8> ExternalRecv;

int main() {
    // A B C D --> ccm
    ChannelEndpoints<u8> &&a_in = CreateChannel<u8>();
    ChannelEndpoints<u8> &&c_in = CreateChannel<u8>();
    ChannelEndpoints<u8> &&d_in = CreateChannel<u8>();

    ChannelEndpoints<InboundMessage> &&in = CreateChannel<InboundMessage>();

    // ccm -> B
    ChannelEndpoints<u8> &&b_out = CreateChannel<u8>();
    // ccm -> C
    ChannelEndpoints<u8> &&c_out = CreateChannel<u8>();
    ChannelEndpoints<u8> &&d_out = CreateChannel<u8>();

    auto a_routine = [&]() {
        string strs[] = {"hello", "world", "goodbye"};
        int ints[] = {9, 28, 22, 64, 3};

        size_t i = 0;
        while (true) {
            AInboundMessage msg(strs[i % 3], ints[i % 5]);
            cout << "A Sending message " << msg._str << " " << msg._num << endl;
            a_in._send << msg;
            this_thread::sleep_for(chrono::seconds(1));
            i++;
        }
    };
    auto b_routine = [&]() {
        while (true) {
            string msg;
            b_out._recv >> msg;
            cout << "B got message " << msg << endl;
            msg.clear();
        }
    };
    auto c_routine = [&]() {
        size_t i = 0;
        while (true) {
            size_t lo, hi;

            lo = rand() % i;
            hi = rand() % (i - lo) + lo;
            CInboundMessage msg(lo, hi);
            cout << "C Sending message " << msg._lo << " " << msg._hi << endl;
            c_in._send << msg;

            vector<int> res;
            c_out._recv >> res;
            cout << "C got message " << res << endl;

            this_thread::sleep_for(chrono::milliseconds(1));
            i++;
        }
    };
    auto d_routine = [&]() {
        while (true) {
            cout << "D sending message " << endl;
            d_in._send << (u8)0;

            int res;
            d_out._recv >> res;
            cout << "D got message " << res << endl;

            this_thread::sleep_for(chrono::milliseconds(1500));
        }
    };

    auto a_pipe = [&]() {
        while (true) {
            AInboundMessage msg;
            a_in._recv >> msg;

            InboundMessage inb;
            inb.sender = InboundMessage::A;
            inb.amsg = msg;
            in._send << inb;
        }
    };
    auto c_pipe = [&]() {
        while (true) {
            CInboundMessage msg;
            c_in._recv >> msg;

            InboundMessage inb;
            inb.sender = InboundMessage::C;
            inb.cmsg = msg;
            in._send << inb;
        }
    };
    auto d_pipe = [&]() {
        while (true) {
            u8 ignored;
            d_in._recv >> ignored;

            InboundMessage inb;
            inb.sender = InboundMessage::D;
            in._send << inb;
        }
    };

    thread athread(a_routine);
    thread bthread(b_routine);
    thread cthread(c_routine);
    thread dthread(d_routine);

    thread apipe(a_pipe);
    thread cpipe(c_pipe);
    thread dpipe(d_pipe);

    while (true) {
        InboundMessage msg;
        in._recv >> msg;

        switch (msg.sender) {
        case InboundMessage::A:
            break;
        case InboundMessage::C:
            break;
        case InboundMessage::D:
            break;
        }
    }

    return 0;
}
