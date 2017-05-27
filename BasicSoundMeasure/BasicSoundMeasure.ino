# define SAMPLE_FREQ 100
# define MICROPHONE_PIN A0

double refVoltage = 1;

void setup() {
  // put your setup code here, to run once:
  // pinMode(2, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(2, HIGH);
  //delay(1000);
  //digitalWrite(2, LOW);
  //delay(1000);
  double sonido = readMicrophoneVoltage();
  Serial.print("Medicion de sonido: ");
  Serial.print(sonido);
  Serial.print("      ");
  Serial.print("Medicion de decibeles: ");
  double volume = 20 * log(sonido / refVoltage);
  Serial.println(volume);
  //delay(100);
}

double readMicrophoneVoltage()
{
    unsigned long startMillis = millis();
    unsigned int peakToPeak = 0;

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    while (millis() - startMillis < SAMPLE_FREQ)
    {
        unsigned int sample = analogRead(MICROPHONE_PIN);
        if (sample < 1024)
        {
            if (sample > signalMax)
            {
                signalMax = sample;
            }
            else if (sample < signalMin)
            {
                signalMin = sample;
            }
        }
    }

    peakToPeak = signalMax - signalMin;
    return (peakToPeak * 5.0) / 1024;
}
