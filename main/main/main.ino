#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Temperature Sensor
#define DHT_TYPE DHT11
#define DHT_PIN D2

// Call Notifier
#define CALL_PIN LED_BUILTIN

// Microphone
#define MICRO_PIN A0
#define MICRO_TOGGLE_PIN D6
#define SOUND_THRESHOLD 512

// WiFi connection data
const char* SSID = "WoloxHQ";
const char* PASSWORD = "WoloxHQ1189";

// Information server
const String REMOTE_SERVER = "http://10.0.0.34:5000";
const String API_URL = "/add_ttss";
const String CONFIG_URL = "/configuration";

// Config request
const size_t MAX_CONTENT_SIZE = 512;
const size_t BUFFER_SIZE = JSON_OBJECT_SIZE(5) + MAX_CONTENT_SIZE;
DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);

// Temperature Sensor
DHT dht(DHT_PIN, DHT_TYPE, 11);

// Sound
int triggerSound = 0;
int lastTemperature = 0;

void setup() {
  // Call led and Temperature sensor
  pinMode(CALL_PIN, OUTPUT);
  pinMode(MICRO_PIN, INPUT);
  pinMode(MICRO_TOGGLE_PIN, INPUT);
  
  // Start console
  Serial.begin(115200);
  Serial.print("\nConnecting");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(String("\nConnected to: ") + SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long start = millis();
  if (start % 1000 == 0) {
    getConfiguration();
    sendValues();
  }
  if (start % 2000 == 0) {
    getTemperature();
  }
  if (start % 500 == 0) {
    readMicrophone(); 
  }
}

void getConfiguration() {
  HTTPClient http;
  http.begin(REMOTE_SERVER + CONFIG_URL);
  
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK ) {
    String payload = http.getString();
    JsonObject& config = jsonBuffer.parseObject(payload);

    digitalWrite(CALL_PIN, config["on_call"] ? LOW : HIGH);
    
    Serial.print("EN CALL: ");
    boolean call = config["on_call"];
    Serial.println(call);
    // TODO: Actualizar variable global así no se prende el led desde este método.
  } else {
    Serial.println("Error al obtener la respuesta.");
  }
  
  http.end();
}

void getTemperature() {
  int temp = dht.readTemperature(false);
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.println(String("Temperature: ") + temp);
    lastTemperature = temp;
  }
  
}

void sendValues() {
  HTTPClient http;
  http.begin(REMOTE_SERVER + API_URL);
  String content = String("{\"volume\": ") + triggerSound + ", \"temperature\":" + lastTemperature + " }";
  http.POST(content);
  triggerSound = 0;
}

void readMicrophone()
{
  unsigned int sample = analogRead(MICRO_PIN);

  if (sample <= SOUND_THRESHOLD && sample != 0) {
    triggerSound = 1;
    Serial.print("Volumen Alto: ");
    Serial.println(sample);
  } 
  //else {
  //  triggerSound = 0;
  //}
  
  /*unsigned long startMillis = millis();
  unsigned int peakToPeak = 0;

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < SAMPLE_FREQ)
  {
      unsigned int sample = analogRead(MICRO_TOGGLE_PIN);
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
  Serial.print("Voltaje?: ");
  Serial.println((peakToPeak * 5.0) / 1024);*/
}
