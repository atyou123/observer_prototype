#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

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

struct MainState {
    vector<int> numbers;
};

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

void loop_iteration(Event event, Channels &channels, MainState &main_state) {
    switch (event) {
    case AReceived: {
        // Deserialization
        size_t b_message_size = 0;
        for (size_t i = 0; i < 8; i++) {
            b_message_size |= channels.ain.front() << (7 - i) * 8;
            channels.ain.pop();
        }

        string b_message;
        for (size_t i = 0; i < b_message_size; i++) {
            b_message.push_back(channels.ain.front());
            channels.ain.pop();
        }

        int num = 0;
        for (size_t i = 0; i < 4; i++) {
            num |= channels.ain.front() << (3 - i) * 8;
            channels.ain.pop();
        }

        size_t bytemask = 0xFF;

        for (size_t i = 0; i < 8; i++) {
            channels.bout.push(b_message_size & bytemask << (7 - i) * 8);
        }

        for (char c : b_message) {
            channels.bout.push(c);
        }

        main_state.numbers.push_back(num);
        break;
    }
    case BReceived: {
        break;
    }
    case CReceived: {
        size_t lo = 0;

        for (size_t i = 0; i < 8; i++) {
            lo |= channels.cin.front() << (7 - i) * 8;
            channels.cin.pop();
        }

        size_t hi = 0;
        for (size_t i = 0; i < 8; i++) {
            hi |= channels.cin.front() << (7 - i) * 8;
            channels.cin.pop();
        }

        for (size_t i = lo; i < hi; i++) {
            channels.cout.push(main_state.numbers[i]);
        }

        break;
    }
    case DReceived: {
        channels.din.pop();

        int total = 0;
        for (int x : main_state.numbers) {
            total += x;
        }
        channels.dout.push(total);

        break;
    }
    }
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

int main() {
    Channels channels;
    MainState state;

    push_size_t(channels.ain, 5);
    push_string(channels.ain, "hello");
    push_int(channels.ain, 10);
    loop_iteration(AReceived, channels, state);

    push_size_t(channels.ain, 3);
    push_string(channels.ain, "abc");
    push_int(channels.ain, 30);
    loop_iteration(AReceived, channels, state);

    push_size_t(channels.ain, 3);
    push_string(channels.ain, "abc");
    push_int(channels.ain, 25);
    loop_iteration(AReceived, channels, state);

    push_size_t(channels.ain, 3);
    push_string(channels.ain, "abc");
    push_int(channels.ain, 9);
    loop_iteration(AReceived, channels, state);

    push_size_t(channels.cin, 1);
    push_size_t(channels.cin, 3);
    loop_iteration(CReceived, channels, state);

    cout << channels.cout.size() << endl;
    print_queue(channels.cout);

    push_size_t(channels.din, 3);
    loop_iteration(DReceived, channels, state);
    print_queue(channels.dout);

    return 0;
}
