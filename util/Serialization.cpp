#include "Serialization.h"

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