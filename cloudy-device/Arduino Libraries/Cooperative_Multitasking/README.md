## Cooperative Multitasking

Cooperative Multitasking is a library that can run multiple functions
(nearly) concurrently or independently from each other. It can run
functions with a delay or when a condition becomes true.

#### Example

The example makes the builtin LED and a second LED connected to pin 2
with a series resistor of 1 kΩ blink with different frequencies.

    #include <CooperativeMultitasking.h>

    CooperativeMultitasking tasks;
    Continuation on1;
    Continuation off1;
    Continuation on2;
    Continuation off2;

    void setup() {
      pinMode(LED_BUILTIN, OUTPUT);
      pinMode(2, OUTPUT);
      tasks.now(on1); // call on1() immediately
      tasks.now(on2); // call on2() immediately
    }

    void loop() {
      tasks.run();
    }

    void on1() {
      digitalWrite(LED_BUILTIN, HIGH);
      tasks.after(1000, off1); // call off1() in 1000 ms
    }

    void off1() {
      digitalWrite(LED_BUILTIN, LOW);
      tasks.after(1000, on1); // call on1() in 1000 ms
    }

    void on2() {
      digitalWrite(2, HIGH);
      tasks.after(1500, off2); // call off2() in 1500 ms
    }

    void off2() {
      digitalWrite(2, LOW);
      tasks.after(1500, on2); // call on2() in 1500 ms
    }

#### Details

Instances of class `CooperativeMultitasking` manage lists of tasks. Each sketch
needs only one instance of `CooperativeMultitasking`:

    #include <CooperativeMultitasking.h>

    CooperativeMultitasking tasks;

A function that should be executed within a task must have a so called
forward declaration:

    Continuation f; // forward declaration

    void f() {
      // your code here
      // you can and should use now, after, ifThen, ifForThen
      // or onlyOneOf here, but don't use delay!
    }

You can use another type of function for checking conditions. These get
a different forward declaration:

    Guard g; // forward declaration

    bool g() {
      // your code here
      // you can use digitalRead, analogRead and comparisons here,
      // but don't use now, after, ifThen, ifForThen, onlyOneOf or delay!
      //
      return true; // or e.g. return analogRead(A1) > 100;
    }

Use `now`, `after`, `ifThen` and `ifForThen` to add tasks to the list:

* With `tasks.now(f);` a call to function `f` is put to the beginning of
  the list. This means the call to `f` is executed immediately.
* With `after(1000, f);` a call to function `f` is executed after waiting for
  1000 milliseconds.
* With `ifThen(g, f);` a call to function `f` is executed, when a call to
  function `g` returns true.
* With `ifForThen(g, 300, f);` a call to function `f` is executed,
  after `g` returns true for at least 300 milliseconds.

Use the method `onlyOneOf` to declare that only one of several tasks should be
executed - the one that becomes active first. You find an example in
[BlinkWhenItsDark.ino](https://bitbucket.org/amotzek/cooperative-multitasking/src/1593ae6fd1b1a750969cecc42296b443ce270f47/examples/BlinkWhenItsDark/BlinkWhenItsDark.ino?at=master&fileviewer=file-view-default).

Typically you add one or more tasks to the list in `setup` and these tasks will
create others.

    void setup() {
      // other code here
      tasks.now(f);
    }

In function `loop` you have to call the method `run`. This makes the tasks
execute when it is their turn.

    void loop() {
      tasks.run(); // don't do something else in loop!
    }
