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
#include <tuple>
#include <utility>

#include "Channel.h"
#include "Fallible.h"
#include "Serialization.h"

using namespace std;

void ChannelExample() {
    // Need rvalue reference && because CreateChannel<int>() is a tempoary value
    ChannelEndpoints<u8> &&epts = CreateChannel<u8>();
    Send<u8> send = epts._send;
    Recv<u8> recv = move(epts._recv);

    // c++ tuples are really buggy, so you can't pass send in as an argument (copied), must pass by reference
    // thread producer(Producer, &send);
    auto producer_routine = [&]() {
        this_thread::sleep_for(chrono::seconds(2));
        // Rapidly send a bunch
        for (i32 i = 0; i < 25; i++) {
            cout << "sending " << i << endl;
            send << i;
        }

        // send once every second, print messages will overlap
        for (i32 i = 0; i < 5; i++) {
            this_thread::sleep_for(chrono::seconds(1));
            cout << "sending " << i << endl;
            send << i;
        }
    };

    auto consumer_routine = [&]() {
        for (i32 i = 0; i < 30; i++) {
            i32 val;
            recv >> val;
            cout << "received " << val << endl;
        }
        cout << "done recving" << endl;
    };

    thread consumer(consumer_routine);
    this_thread::sleep_for(chrono::seconds(1));
    thread producer(producer_routine);

    producer.join();
    consumer.join();
}