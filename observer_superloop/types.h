#ifndef TYPES_H
#define TYPES_H

#include "../util/Channel.h"
#include <array>
#include <vector>


// Implementation dependent, this code is not meant to be run on the ccm, replace these typedefs with int8u... upon copying
typedef unsigned char u8;
typedef char i8;
typedef unsigned short u16;
typedef short i16;
typedef unsigned int u32;
typedef int i32;
typedef unsigned long u64;
typedef long i64;

Send<u8> &operator<<(Send<u8> &send, u16 item);
Recv<u8> &operator>>(Recv<u8> &recv, u16 &dest);
Send<u8> &operator<<(Send<u8> &send, u32 item);
Recv<u8> &operator>>(Recv<u8> &recv, u32 &dest);
Send<u8> &operator<<(Send<u8> &send, u64 item);
Recv<u8> &operator>>(Recv<u8> &recv, u64 &dest);
Send<u8> &operator<<(Send<u8> &send, i8 item);
Recv<u8> &operator>>(Recv<u8> &recv, i8 &dest);
Send<u8> &operator<<(Send<u8> &send, i16 item);
Recv<u8> &operator>>(Recv<u8> &recv, i16 &dest);
Send<u8> &operator<<(Send<u8> &send, i32 item);
Recv<u8> &operator>>(Recv<u8> &recv, i32 &dest);
Send<u8> &operator<<(Send<u8> &send, i64 item);
Recv<u8> &operator>>(Recv<u8> &recv, i64 &dest);

/*
 * A wrapper around a class that has implicit copy and move semantics implemented (e.g. std::string, std::vector,
 * std::shared_ptr) that blocks it from being implicitly copied. This class can be copied explicitly using clone()
 * This is useful for ensuring heap-allocated data that could be large isn't copied accidentally
 */
template <typename T> class CopyBlocked {
  public:
    // Must move T in to construct
    CopyBlocked(T &&val) : _val(move(val)) {}

    CopyBlocked(CopyBlocked &other) = delete;
    CopyBlocked(CopyBlocked &&other) : _val(move(other._val)) {}

    CopyBlocked clone() { return CopyBlocked(_val); }

  private:
    /*
     * Since T might be an expensive type to copy, we ensure that initialization will not unnecessarily copy T by making
     * this lvalue copy constructor private
     */
    CopyBlocked(T &val) : _val(val) {}
    T _val;
};

struct AInboundMessage {
    CopyBlocked<string> _str;
    int _num;
    AInboundMessage(CopyBlocked<string> &&str, int num) : _str(move(str)), _num(num) {}
    AInboundMessage(string &&str, int num) : _str(move(str)), _num(num) {}
    AInboundMessage(const char *str, int num) : _str(str), _num(num) {}
    AInboundMessage(AInboundMessage &&other) : _str(move(other._str)), _num(other._num) {}

    AInboundMessage clone() { return AInboundMessage(_str.clone(), _num); }
};

struct CInboundMessage {
    size_t _lo;
    size_t _hi;
    CInboundMessage() {}
    CInboundMessage(size_t lo, size_t hi) : _lo(lo), _hi(hi) {}
};

struct InboundMessage {
    enum Sender { A, C, D };

    Sender sender;

    AInboundMessage amsg;
    CInboundMessage cmsg;
};

Send<u8> &operator<<(Send<u8> &send, string item);
Recv<u8> &operator>>(Recv<u8> &recv, string &dest);

Send<u8> &operator<<(Send<u8> &send, AInboundMessage item);
Recv<u8> &operator>>(Recv<u8> &recv, AInboundMessage &dest);
Send<u8> &operator<<(Send<u8> &send, CInboundMessage item);
Recv<u8> &operator>>(Recv<u8> &recv, CInboundMessage &dest);

Send<u8> &operator<<(Send<u8> &send, vector<int> item);
Recv<u8> &operator>>(Recv<u8> &recv, vector<int> &dest);
ostream &operator<<(ostream &os, vector<int> &vec);


#endif