static const uint8_t kCurrentPin = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(kCurrentPin, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(kCurrentPin));
  delay(500);
}
