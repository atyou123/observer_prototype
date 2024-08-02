#ifndef CHANNEL_H
#define CHANNEL_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include "Fallible.h"

using namespace std;

// Note: template functions/classes need to be defined in header files and included

/**
 * The implementation is mostly the same as a subset of Rust channels
 * - The Channel object is not supposed to be used at all, instead you use the Send and Recv objects initialized using
 * CreatedChannel<T>()
 * - multi producer single consumer. If you want to have multiple consumers, simply send the message through multiple
 * different channels
 *      - Therefore the Recv object is move only and must be unique like a unique_ptr
 * - Messages passed through should be moved if they are not stack allocated
 * - There can be multiple producers (Send), so the object can be copied, when all copies go out of scope the channel
 * closes and the receiver is notified
 * - recv throws an exception if the channel is closed alternatively an option can be used
 */

template <typename T> struct Channel {
    queue<T> _queue;
    condition_variable _condition_variable;
    mutex _mutex;
    atomic<size_t> _send_count;
};

template <typename T> class Send {
  public:
    Send(shared_ptr<Channel<T>> chan) : _chan(chan) { _chan->_send_count++; }

    // Move construction copies the Send, need to keep track of the amount of
    // senders
    Send(Send &other) : _chan(other._chan) { _chan->_send_count++; }

    ~Send() {
        size_t res = --_chan->_send_count;
        if (res == 0) {
            _chan->_condition_variable.notify_one();
        }
    }

    void send(T object) {
        lock_guard<mutex> lock(_chan->_mutex);
        _chan->_queue.push(object);
        _chan->_condition_variable.notify_one();
    }

  private:
    shared_ptr<Channel<T>> _chan;
};

template <typename T> class Recv {
  public:
    Recv(shared_ptr<Channel<T>> chan) : _chan(chan) {}

    Recv(Recv &&old) : _chan(old._chan) { old._chan = nullptr; }

    struct ChannelClosed {};

    T recv() {
        unique_lock<mutex> lock(_chan->_mutex);
        if (_chan->_queue.empty()) {
            if (_chan->_send_count == 0) {
                throw ChannelClosed();
            }
            _chan->_condition_variable.wait(lock);
            if (_chan->_send_count == 0) {
                throw ChannelClosed();
            }
        }
        T item = _chan->_queue.front();
        _chan->_queue.pop();
        return item;
    }

  private:
    shared_ptr<Channel<T>> _chan;
};

template <typename T> struct ChannelEndpoints {
    Send<T> _send;
    Recv<T> _recv;

    ChannelEndpoints(Send<T> send, Recv<T> recv) : _send(send), _recv(move(recv)) {}

    ChannelEndpoints(ChannelEndpoints &&other) : _send(other._send), _recv(move(other._recv)) {}
};

template <typename T> ChannelEndpoints<T> CreateChannel() {
    shared_ptr<Channel<T>> chan = make_shared<Channel<T>>();
    Send<T> send(chan);
    Recv<T> recv(chan);
    ChannelEndpoints<T> epts(send, move(recv));
    return epts;
}

template <typename T> Send<T> &operator<<(Send<T> &send, T item) {
    send.send(item);
    return send;
}

template <typename T> Recv<T> &operator>>(Recv<T> &recv, T &dest) {
    dest = recv.recv();
    return recv;
}

void ChannelExample();

#endif