#define INTERVAL 997
#define DELAY 100

int pins[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };
int values[8] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW };
int report = false;
unsigned long last_report;

void setup() {
  int i;
  
  Serial.begin(115200);
  
  for (i = 0; i < 8; i++)
    pinMode(pins[i], INPUT);
  
  last_report = millis();
}

void loop() {
  unsigned long now, elapsed = 0;
  int tmp;
  int i;

  while (!Serial)
    last_report = millis();
  
  now = millis();
  elapsed = now - last_report;
  
  for (i = 0; i < 8; i++) {
    tmp = digitalRead(pins[i]);
    if (tmp != values[i])
      report = true;
    values[i] = tmp;
  }
   
  if (elapsed >= INTERVAL || (report && elapsed >= DELAY)) {
    int value = 0;
    
    for (i = 0; i < 8; i++)
      if (values[i])
        value |= (1 << i);
    
    Serial.print("{ \"now\": ");
    Serial.print(now);
    Serial.print(", \"value\": ");
    Serial.print(value);
    Serial.println(" }");
    Serial.flush();
    
    last_report = now;
    report = false;
  }
}
