/*
	pinmonitor - Monitor the status of pins with serial output
	Copyright 2013-2014  Simon Arlott

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Bounce.h>
#define INTERVAL 4992
#define DELAY 1
#define NUM 11
#define DEBOUNCE 5

int pins[NUM] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
int active[NUM] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW };
Bounce *values[NUM] = {};
unsigned long ts[NUM] = {};
int report = false;
unsigned long last_report;

void setup() {
  int i;

  Serial.begin(115200);

  for (i = 0; i < NUM; i++) {
    pinMode(pins[i], INPUT_PULLUP);
    values[i] = new Bounce(pins[i], DEBOUNCE);
    values[i]->write(HIGH);
  }
  delay(DEBOUNCE);

  last_report = millis();
}

void loop() {
  unsigned long now, elapsed = 0;
  int i;

  while (!Serial)
    last_report = millis() - INTERVAL;

  for (i = 0; i < NUM; i++) {
    report |= values[i]->update();
  }

  now = millis();
  elapsed = now - last_report;

  if (elapsed >= INTERVAL || (report && elapsed >= DELAY)) {
    int value = 0;

    for (i = 0; i < NUM; i++) {
      if (values[i]->read() == active[i])
        value |= (1 << i);
    }

    if (Serial) {
      Serial.print("V");
      Serial.println(value);
    }

    last_report = now;
    report = false;
  }
}
