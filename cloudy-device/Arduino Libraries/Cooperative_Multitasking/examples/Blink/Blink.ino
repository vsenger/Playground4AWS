/*
 * Make the builtin LED blink.
 *
 * Copyright (C) 2018 Andreas Motzek andreas-motzek@t-online.de
 *
 * You can use, redistribute and/or modify this file under the terms of the Modified Artistic License.
 * See http://simplysomethings.de/open+source/modified+artistic+license.html for details.
 *
 * This file is distributed in the hope that it will be useful, but without any warranty; without even
 * the implied warranty of merchantability or fitness for a particular purpose.
 */

#include <CooperativeMultitasking.h>

CooperativeMultitasking tasks;
Continuation on; // forward declaration
Continuation off; // forward declaration

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  tasks.now(on); // call on() now
}

void loop() {
  tasks.run();
}

void on() {
  digitalWrite(LED_BUILTIN, HIGH);
  tasks.after(1000, off); // call off() in 1000 ms
}

void off() {
  digitalWrite(LED_BUILTIN, LOW);
  tasks.after(1000, on); // call on() in 1000 ms
}
