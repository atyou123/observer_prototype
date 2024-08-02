#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "Channel.h"

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

#endif