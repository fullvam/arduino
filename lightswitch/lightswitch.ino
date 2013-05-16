/*
	lightswitch - Apply pulses to light switch impulse latching relays from serial input
	Copyright 2013  Simon Arlott

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

enum state {
  idle,
  pulse_on,
  pulse_off
};

#define NUM 2
#define PULSE_ON 200
#define PULSE_OFF 300

char *names[NUM] = { "LEFT", "RIGHT" };
unsigned char cmds[NUM] = { 'l', 'r' };
int pins[NUM] = { 3, 12 };
int pulses[NUM];
state state[NUM];
unsigned long ts[NUM];

void setup() {
  int i;

  Serial.begin(115200);

  for (i = 0; i < NUM; i++) {
    digitalWrite(pins[i], LOW);
    pinMode(pins[i], OUTPUT);
    pulses[i] = 0;
    state[i] = idle;
  }
}

void loop() {
  int i;

  for (i = 0; i < NUM; i++) {
    if (pulses[i] > 0) {
      switch (state[i]) {
        case idle:
          digitalWrite(pins[i], HIGH);
          ts[i] = millis();
          state[i] = pulse_on;

          if (Serial) {
            Serial.print(millis());
            Serial.print(" ");
            Serial.print(names[i]);
            Serial.println(" idle -> pulse_on");
          }
          break;

        case pulse_on:
          if (millis() - ts[i] >= PULSE_ON) {
            digitalWrite(pins[i], LOW);
            ts[i] = millis();
            state[i] = pulse_off;

            if (Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.println(" pulse_on -> pulse_off");
            }
          }
          break;

        case pulse_off:
          if (millis() - ts[i] >= PULSE_OFF) {
            state[i] = idle;

            if (Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.println(" pulse_off -> idle");
            }

            pulses[i]--;

            if (Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.print(" --pulses = ");
              Serial.println(pulses[i]);
            }
          }
          break;
      }
    } else {
      digitalWrite(pins[i], LOW);
      pulses[i] = 0;
      state[i] = idle;
    }
  }

  if (Serial.available() > 0) {
    int data = Serial.read();

    for (i = 0; i < NUM; i++) {
      if (data == cmds[i]) {
        pulses[i]++;

        if (Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.print(" ++pulses = ");
              Serial.println(pulses[i]);
        }
      }
    }
  }
}
