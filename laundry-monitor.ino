static const uint8_t kCurrentPin = 13;
static const uint8_t kLedPin = 2;

void setup() {
  Serial.begin(115200);

  pinMode(kCurrentPin, INPUT);
  pinMode(kLedPin, OUTPUT);

  digitalWrite(kLedPin, LOW);
}

uint32_t average;
int printCount = 0;

void loop() {
  average = (average * 3 + analogRead(kCurrentPin)) / 4;
  digitalWrite(kLedPin, average > 0);
  delay(10);

  printCount++;
  if (printCount > 10) {
    printCount = 0;
    Serial.println(average);
  }
}
