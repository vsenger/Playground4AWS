/*
 * Make the builtin LED blink, but only when it's dark.
 *
 * Connect an analog light sensor with the Arduino: GND to GND,
 * VCC to VCC, SIG to A1.
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
Continuation light; // forward declaration
Guard isDark; // forward declaration
Continuation on; // forward declaration
Continuation off; // forward declaration
Guard isLight; // forward declaration

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  tasks.now(light); // call light() now
}

void loop() {
  tasks.run();
}

void light() {
  tasks.ifForThen(isDark, 5000, on); // if isDark() for 5000 ms then call on()
}

bool isDark() {
  return analogRead(A1) < 300;
}

void on() {
  digitalWrite(LED_BUILTIN, HIGH);
  tasks.after(1000, off); // call off() in 1000 ms
}

void off() {
  digitalWrite(LED_BUILTIN, LOW);
  auto task1 = tasks.after(1000, on); // call on() in 1000 ms
  auto task2 = tasks.ifForThen(isLight, 0, light); // if isLight() then call light()
  tasks.onlyOneOf(task1, task2); // do either task1 or task2
}

bool isLight() {
  return analogRead(A1) >= 400;
}
