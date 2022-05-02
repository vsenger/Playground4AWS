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

#include "CooperativeMultitasking.h"

CooperativeMultitasking::CooperativeMultitasking(int _capacity) {
  capacity = _capacity;
  heap = new Task*[capacity + 1];
  count = 0;
  cycle = 100 >> 1;
}

CooperativeMultitasking::~CooperativeMultitasking() {
  for (int i = 1; i < count; i++) {
    delete heap[i];
    heap[i] = NULL;
  }
  //
  delete[] heap;
  heap = NULL;
  capacity = 0;
  count = 0;
}

CooperativeMultitasking::Task* CooperativeMultitasking::now(Continuation* continuation, int priority) {
  if (!continuation || isFull()) return NULL;
  //
  Task* task = create(millis() >> 1, priority, continuation);
  add(task);
  //
  return task;
}

CooperativeMultitasking::Task* CooperativeMultitasking::after(unsigned long duration, Continuation* continuation, int priority) {
  if (!continuation || isFull()) return NULL;
  //
  Task* task = create((millis() >> 1) + (duration >> 1), priority, continuation);
  add(task);
  //
  return task;
}

CooperativeMultitasking::Task* CooperativeMultitasking::ifForThen(Guard* guard, unsigned long duration, Continuation* continuation, int priority) {
  if (!guard || !continuation || isFull()) return NULL;
  //
  Task* task = create(millis() >> 1, priority, continuation, guard, duration >> 1, duration >> 1);
  add(task);
  //
  return task;
}

CooperativeMultitasking::Task* CooperativeMultitasking::ifThen(Guard* guard, Continuation* continuation, int priority) {
  return ifForThen(guard, 0, continuation, priority);
}

void CooperativeMultitasking::onlyOneOf(Task* task1, Task* task2) {
  if (task1 && task2) {
    task1->sibling1 = task2;
    task2->sibling1 = task1;
  }
}

void CooperativeMultitasking::onlyOneOf(Task* task1, Task* task2, Task* task3) {
  if (task1 && task2 && task3) {
    task1->sibling1 = task2;
    task1->sibling2 = task3;
    task2->sibling1 = task1;
    task2->sibling2 = task3;
    task3->sibling1 = task1;
    task3->sibling2 = task2;
  } else {
    onlyOneOf(task1, task2);
    onlyOneOf(task1, task3);
    onlyOneOf(task2, task3);
  }
}

void CooperativeMultitasking::onlyOneOf(Task* task1, Task* task2, Task* task3, Task* task4) {
  if (task1 && task2 && task3 && task4) {
    task1->sibling1 = task2;
    task1->sibling2 = task3;
    task1->sibling3 = task4;
    task2->sibling1 = task1;
    task2->sibling2 = task3;
    task2->sibling3 = task4;
    task3->sibling1 = task1;
    task3->sibling2 = task2;
    task3->sibling3 = task4;
    task4->sibling1 = task1;
    task4->sibling2 = task2;
    task4->sibling3 = task3;
  } else {
    onlyOneOf(task1, task2, task3);
    onlyOneOf(task1, task2, task4);
    onlyOneOf(task1, task3, task4);
    onlyOneOf(task2, task3, task4);
  }
}

int CooperativeMultitasking::available() {
  return count;
}

void CooperativeMultitasking::run() {
  unsigned long now = millis() >> 1;
  //
  if (last > now) handleOverflow();
  //
  last = now;
  Task* task = extract(1);
  //
  if (!task) {
    wait(cycle);
    //
    return;
  }
  //
  if (now < task->when) wait(task->when - now);
  //
  if (task->guard) {
    bool result = task->guard();
    //
    if (result) {
      if (task->remaining > cycle) {
        task->remaining -= cycle;
        task->when = now + cycle;
        add(task);
        //
        return;
      }
    } else {
      task->remaining = task->duration;
      task->when = now + cycle;
      add(task);
      //
      return;
    }
  }
  //
  if (task->sibling1) {
    remove(task->sibling1);
    delete task->sibling1;
  }
  //
  if (task->sibling2) {
    remove(task->sibling2);
    delete task->sibling2;
  }
  //
  if (task->sibling3) {
    remove(task->sibling3);
    delete task->sibling3;
  }
  //
  Continuation* continuation = task->continuation;
  delete task;
  continuation();
}

void CooperativeMultitasking::handleOverflow() {
  for (int i = 1; i <= count; i++) {
    Task* task = heap[i];
    //
    if (task->when & 0x80000000) {
      task->when &= 0x7fffffff; // clear overflow
    } else {
      task->when = 0; // was due before the overflow
    }
  }
  //
  for (int i = 1; i <= count; i++) {
    bottomUp(i); // restore heap
  }
}

CooperativeMultitasking::Task* CooperativeMultitasking::create(unsigned long when, int priority, Continuation* continuation, Guard* guard, unsigned long duration, unsigned long remaining) {
  Task* task = new Task(); // std::nothrow is default
  //
  if (task) {
    task->when = when;
    task->priority = priority;
    task->continuation = continuation;
    task->guard = guard;
    task->duration = duration;
    task->remaining = remaining;
    task->sibling1 = NULL;
    task->sibling2 = NULL;
    task->sibling3 = NULL;
  }
  //
  return task;
}

void CooperativeMultitasking::add(Task* task) {
  if (task) {
    heap[++count] = task;
    bottomUp(count);
  }
}

CooperativeMultitasking::Task* CooperativeMultitasking::extract(int i) {
  if (isOutside(i)) return NULL;
  //
  Task* task = heap[i];
  topDown(i);
  //
  return task;
}

void CooperativeMultitasking::remove(const Task* task) {
  int i = find(task);
  //
  if (isInside(i)) topDown(i);
}

int CooperativeMultitasking::find(const Task* task, int i) {
  while (!isLeaf(i)) {
    if (heap[i] == task) return i;
    //
    if (isBefore(task, heap[i])) return 0;
    //
    int r = rightChild(i);
    //
    if (isInside(r))
    {
      int j = find(task, r);
      //
      if (isInside(j)) return j;
    }
    //
    i = leftChild(i);
  }
  //
  if (heap[i] == task) return i;
  //
  return 0;
}

void CooperativeMultitasking::bottomUp(int i) {
  while (hasParent(i)) {
    int p = parent(i);
    //
    if (isBefore(heap[i], heap[p])) {
      Task* task = heap[i];
      heap[i] = heap[p];
      heap[p] = task;
      i = p;
    } else {
      return;
    }
  }
}

void CooperativeMultitasking::topDown(int i) {
  while (!isLeaf(i)) {
    int l = leftChild(i);
    int r = rightChild(i);
    //
    if (isInside(r) && isBefore(heap[r], heap[l])) {
      heap[i] = heap[r];
      i = r;
    } else {
      heap[i] = heap[l];
      i = l;
    }
  }
  //
  if (i == count) {
    heap[i] = NULL;
    count--;
  } else {
    heap[i] = heap[count];
    heap[count--] = NULL;
    bottomUp(i);
  }
}

bool CooperativeMultitasking::isBefore(const Task* task1, const Task* task2) {
  if (task1->when < task2->when) return true;
  //
  if (task1->when == task2->when && task1->priority > task2->priority) return true;
  //
  return false;
}

inline bool CooperativeMultitasking::isFull() {
  return count >= capacity;
}

inline bool CooperativeMultitasking::isOutside(int i) {
  return i < 1 || i > count;
}

inline bool CooperativeMultitasking::isInside(int i) {
  return i >= 1 && i <= count;
}

inline bool CooperativeMultitasking::isLeaf(int i) {
  return i > (count >> 1);
}

inline bool CooperativeMultitasking::hasParent(int i) {
  return i > 1;
}

inline int CooperativeMultitasking::parent(int i) {
  return i >> 1;
}

inline int CooperativeMultitasking::leftChild(int i) {
  return i << 1;
}

inline int CooperativeMultitasking::rightChild(int i) {
  return (i << 1) + 1;
}