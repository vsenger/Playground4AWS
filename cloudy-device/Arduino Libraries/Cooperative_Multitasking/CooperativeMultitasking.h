/*
 * Copyright (C) 2018 Andreas Motzek andreas-motzek@t-online.de
 *
 * This file is part of the Cooperative Multitasking package.
 *
 * You can use, redistribute and/or modify this file under the terms of the Modified Artistic License.
 * See http://simplysomethings.de/open+source/modified+artistic+license.html for details.
 *
 * This file is distributed in the hope that it will be useful, but without any warranty; without even
 * the implied warranty of merchantability or fitness for a particular purpose.
 */

#ifndef CooperativeMultitasking_h
#define CooperativeMultitasking_h

#include "Arduino.h"

typedef void Continuation(); // Continuation task; void task() { ... }

typedef bool Guard(); // Guard test; bool test() { return false; }

class CooperativeMultitasking {
  private:
    struct Task {
      unsigned long when;
      int priority;
      Continuation* continuation;
      Guard* guard;
      unsigned long duration;
      unsigned long remaining;
      Task* sibling1;
      Task* sibling2;
      Task* sibling3;
    };

    int capacity;
    Task** heap;
    int count;
    unsigned long cycle;
    unsigned long last;

    void handleOverflow();
    static Task* create(unsigned long when, int priority, Continuation* continuation, Guard* guard = NULL, unsigned long duration = 0, unsigned long remaining = 0);
    void add(Task* task);
    Task* extract(int index);
    void remove(const Task* task);
    int find(const Task* task, int index = 1);
    void bottomUp(int index);
    void topDown(int index);
    static bool isBefore(const Task* task1, const Task* task2);
    inline bool isFull();
    inline bool isOutside(int index);
    inline bool isInside(int index);
    inline bool isLeaf(int index);
    static inline bool hasParent(int index);
    static inline int parent(int index);
    static inline int leftChild(int index);
    static inline int rightChild(int index);

#ifdef _ARDUINO_LOW_POWER_H_
    static void wait(unsigned long duration) { LowPower.idle(duration << 1); }
#else
    static void wait(unsigned long duration) { delay(duration << 1); }
#endif

  public:
    CooperativeMultitasking(int capacity = 32);
    virtual ~CooperativeMultitasking();
    Task* now(Continuation continuation, int priority = 0);
    Task* after(unsigned long duration, Continuation* continuation, int priority = 0);
    Task* ifForThen(Guard* guard, unsigned long duration, Continuation* continuation, int priority = 0);
    Task* ifThen(Guard* guard, Continuation* continuation, int priority = 0);
    void onlyOneOf(Task* task1, Task* task2);
    void onlyOneOf(Task* task1, Task* task2, Task* task3);
    void onlyOneOf(Task* task1, Task* task2, Task* task3, Task* task4);
    int available();
    void run();
};

#endif