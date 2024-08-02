#include "../observer_superloop/types.h"
#include "../util/Channel.h"
#include <vector>


struct InternalMessage {
    virtual void nothing() {}
};

struct Message {
    enum Tag {
        External, Internal
    };

    Message(Tag tag, u8 ext) : external(ext), tag(tag) {}

    Tag tag;
    union {
        u8 external;
        InternalMessage *internal;
    };
};

struct StringMsg : InternalMessage {
    CopyBlocked<string> _message;

    StringMsg(CopyBlocked<string> &&msg) : _message(move(msg)) {}
};

struct QueryBounds : InternalMessage {
    CInboundMessage msg;
};

struct QueryResult : InternalMessage {
    CopyBlocked<vector<int>> vec;
};

struct UnitMsg : InternalMessage {};

class A;
class B;
class C;
class D;

class A {
  public:
    void ThreadBody() {}

    Send<InternalMessage> b_out;
    Send<InternalMessage> c_out;
    Send<InternalMessage> d_out;

    Recv<Message> in;

    Send<u8> ext_out;
    Recv<u8> ext_in;
};

class B {
  public:
    void ThreadBody() {}

    Recv<InternalMessage> a_in;

    Send<u8> ext_out;
};

class C {
  public:
    void ThreadBody() {}

    Send<InternalMessage> a_out;
    Recv<InternalMessage> a_in;

    Send<u8> ext_out;
    Recv<u8> ext_in;
};

class D{
    public:
    void ThreadBody() {
        while (true) {
            InternalMessage msg;
        }
    }

    Send<InternalMessage> a_out;

    Send<u8> ext_out;
    Recv<u8> ext_in;
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
