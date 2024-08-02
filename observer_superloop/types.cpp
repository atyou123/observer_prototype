#include "types.h"

Send<u8> &operator<<(Send<u8> &send, u16 item) {
    u8 *view = (u8 *)&item;
    send.send(view[0]);
    send.send(view[1]);
    return send;
}

Recv<u8> &operator>>(Recv<u8> &recv, u16 &dest) {
    u8 *view = (u8 *)&dest;
    view[0] = recv.recv();
    view[1] = recv.recv();
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, u32 item) {
    u8 *view = (u8 *)&item;
    for (size_t i = 0; i < 4; i++) {
        send.send(view[0]);
    }
    return send;
}

Recv<u8> &operator>>(Recv<u8> &recv, u32 &dest) {
    u8 *view = (u8 *)&dest;
    for (size_t i = 0; i < 4; i++) {
        view[i] = recv.recv();
    }
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, u64 item) {
    u8 *view = (u8 *)&item;
    for (size_t i = 0; i < 8; i++) {
        send.send(view[0]);
    }
    return send;
}

Recv<u8> &operator>>(Recv<u8> &recv, u64 &dest) {
    u8 *view = (u8 *)&dest;
    for (size_t i = 0; i < 8; i++) {
        view[i] = recv.recv();
    }
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, i8 item) {
    send.send((u8)item);
    return send;
}

Recv<u8> &operator>>(Recv<u8> &recv, i8 &dest) {
    dest = (i8)recv.recv();
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, i16 item) {
    u8 *view = (u8 *)&item;
    for (size_t i = 0; i < 2; i++) {
        send.send(view[0]);
    }
    return send;
}

Recv<u8> &operator>>(Recv<u8> &recv, i16 &dest) {
    u8 *view = (u8 *)&dest;
    for (size_t i = 0; i < 2; i++) {
        view[i] = recv.recv();
    }
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, i32 item) {
    u8 *view = (u8 *)&item;
    for (size_t i = 0; i < 4; i++) {
        send.send(view[0]);
    }
    return send;
}

Recv<u8> &operator>>(Recv<u8> &recv, i32 &dest) {
    u8 *view = (u8 *)&dest;
    for (size_t i = 0; i < 4; i++) {
        view[i] = recv.recv();
    }
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, i64 item) {
    u8 *view = (u8 *)&item;
    for (size_t i = 0; i < 8; i++) {
        send.send(view[0]);
    }
    return send;
}

Recv<u8> &operator>>(Recv<u8> &recv, i64 &dest) {
    u8 *view = (u8 *)&dest;
    for (size_t i = 0; i < 8; i++) {
        view[i] = recv.recv();
    }
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, string item) {
    send << item.length();
    for (char c : item) {
        send << c;
    }
    return send;
}
Recv<u8> &operator>>(Recv<u8> &recv, string &dest) {
    size_t len;
    recv >> len;
    for (size_t i = 0; i < len; i++) {
        dest.push_back(recv.recv());
    }
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, AInboundMessage item) {
    send << item._str;
    send << item._num;
    return send;
}
Recv<u8> &operator>>(Recv<u8> &recv, AInboundMessage &dest) {
    recv >> dest._str;
    recv >> dest._num;
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, CInboundMessage item) {
    send << item._lo;
    send << item._hi;
    return send;
}
Recv<u8> &operator>>(Recv<u8> &recv, CInboundMessage &dest) {
    recv >> dest._lo;
    recv >> dest._hi;
    return recv;
}

Send<u8> &operator<<(Send<u8> &send, vector<int> item) {
    send << item.size();
    for (int x : item) {
        send << x;
    }
    return send;
}
Recv<u8> &operator>>(Recv<u8> &recv, vector<int> &dest) {
    size_t len;
    recv >> len;
    for (size_t i = 0; i < len; i++) {
        dest.push_back(recv.recv());
    }
    return recv;
}

ostream &operator<<(ostream &os, vector<int> &vec) {
    os << "[ ";
    for (int x : vec) {
        os << x << ' ';
    }
    os << ']';
    return os;
}
