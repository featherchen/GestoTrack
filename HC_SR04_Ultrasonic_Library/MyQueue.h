#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <algorithm>
#include <assert.h>

template <typename T, int N, int W>
struct MyQueue {
    static constexpr int n = N;
    static constexpr int window = W;

    bool valid = false;
    T a[n];
    int front = 0;
    int back = 0;

    MyQueue() {
        assert(window * 2 <= n);
        memset(a, 0, sizeof(a));
    }

    void reset() {
        front = back = 0;
    }

    int size() {
        return back - front;
    }

    bool ready() {
        return valid;
    }

    T& operator[](int i) {
        assert(0 <= i && i < size());
        return a[front + i];
    }

    void push(T x) {
        if (back == n) {
            assert(size() == window);
            std::copy(a + front, a + n, a);
            front = 0, back = window;
        }

        a[back++] = x;

        int sz = size();
        
        if (sz >= window) {
            valid = true;
            if (sz > window) {
                front++;
            }
        }
    }

    T last() {
        return a[front + window - 1];
    }
};

#endif // QUEUE_H