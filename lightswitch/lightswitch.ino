/*
	lightswitch - Apply pulses to light switch impulse latching relays from serial input
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

enum state {
  idle,
  pulse_on,
  pulse_off
};

#define NUM 2
#define PULSE_ON 200
#define PULSE_OFF 300
#define STATUS_CMD 's'

char *names[NUM] = { "LEFT", "RIGHT" };
unsigned char vcmds[NUM] = { 'l', 'r' };
unsigned char scmds[NUM] = { 'L', 'R' };
int pins[NUM] = { 3, 12 };
unsigned int pulses[NUM];
unsigned int total[NUM];
state state[NUM];
unsigned long ts[NUM];
boolean verbose[NUM];

void setup() {
  int i;

  Serial.begin(115200);

  for (i = 0; i < NUM; i++) {
    digitalWrite(pins[i], LOW);
    pinMode(pins[i], OUTPUT);
    pulses[i] = 0;
    total[i] = 0;
    state[i] = idle;
    verbose[i] = false;
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

          if (verbose[i] && Serial) {
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

            if (verbose[i] && Serial) {
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

            if (verbose[i] && Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.println(" pulse_off -> idle");
            }

            pulses[i]--;
            total[i]++;

            if (verbose[i] && Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.print(" --pulses = ");
              Serial.println(pulses[i]);
            }

            if (pulses[i] == 0) {
              verbose[i] = false;
            }
          }
          break;
      }
    }
  }

  if (Serial.available() > 0) {
    int data = Serial.read();

    for (i = 0; i < NUM; i++) {
      if (data == vcmds[i]) {
        pulses[i]++;
        verbose[i] = true;

        if (Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.print(" ++pulses = ");
              Serial.println(pulses[i]);
        }
      }

      if (data == scmds[i]) {
        pulses[i]++;
      }
    }

    if (data == STATUS_CMD) {
      for (i = 0; i < NUM; i++) {
        if (Serial) {
              Serial.print(millis());
              Serial.print(" ");
              Serial.print(names[i]);
              Serial.print(" pulses=");
              Serial.print(pulses[i]);
              Serial.print(" ts=");
              Serial.print(ts[i]);
              Serial.print(" state=");
              switch (state[i]) {
                case idle:
                  Serial.print("idle");
                  break;

                case pulse_on:
                  Serial.print("pulse_on");
                  break;

                case pulse_off:
                  Serial.print("pulse_off");
                  break;

                default:
                  Serial.print(state[i]);
                  break;
              }
              Serial.print(" total=");
              Serial.println(total[i]);
        }
      }
    }
  } else {
    delay(1);
  }
}
