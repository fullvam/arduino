#include <Bounce.h>
#define INTERVAL 992
#define DELAY 100

int pins[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };
Bounce *values[8] = {};
int report = false;
unsigned long last_report;

void setup() {
  int i;
  
  Serial.begin(115200);
  
  for (i = 0; i < 8; i++) {
    pinMode(pins[i], INPUT);
    values[i] = new Bounce(pins[i], 5);
    values[i]->write(LOW);
  }
  
  last_report = millis();
}

void loop() {
  unsigned long now, elapsed = 0;
  int tmp;
  int i;

  while (!Serial)
    last_report = millis();

  for (i = 0; i < 8; i++)
    report |= values[i]->update();

  now = millis();
  elapsed = now - last_report;
   
  if (elapsed >= INTERVAL || (report && elapsed >= DELAY)) {
    int value = 0;

    for (i = 0; i < 8; i++)
      if (values[i]->read())
        value |= (1 << i);

    if (Serial) {
      Serial.print("{ \"now\": ");
      Serial.print(now);
      Serial.print(", \"value\": ");
      Serial.print(value);
      Serial.println(" }");
    }
    
    last_report = now;
    report = false;
  }
}
