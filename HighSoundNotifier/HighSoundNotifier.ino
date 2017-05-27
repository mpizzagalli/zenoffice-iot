# define BIG_SAMPLE_FREQ 1000
# define SAMPLE_FREQ 50
# define HIGH_SOUND_OCURRENCIES_THRESHOLD 5
# define MICROPHONE_DIGITAL_PIN D0

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int count = 0;
  while (millis() - startMillis < BIG_SAMPLE_FREQ) {
    int value = digitalRead(MICROPHONE_DIGITAL_PIN);
    Serial.print(value);
    if (value > 0) {
      count++;
    }
    delay(SAMPLE_FREQ);
  }
  Serial.println("");
  Serial.println(count);
  if (count > HIGH_SOUND_OCURRENCIES_THRESHOLD) {
    Serial.println("Volumen alto, SHHHHHH!");
  } else {
    Serial.println("Volumen normal =)");
  }
}
