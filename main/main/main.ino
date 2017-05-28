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

// WiFi connection data
const char* SSID = "WoloxHQ";
const char* PASSWORD = "WoloxHQ1189";

// Information server
const String REMOTE_SERVER = "http://10.0.0.110:5000";
const String API_URL = "/add_ttss";
const String CONFIG_URL = "/configuration";

// Config request
const size_t MAX_CONTENT_SIZE = 512;
const size_t BUFFER_SIZE = JSON_OBJECT_SIZE(5) + MAX_CONTENT_SIZE;
DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);

// Temperature Sensor
DHT dht(DHT_PIN, DHT_TYPE, 11);

void setup() {
  // Call led and Temperature sensor
  pinMode(CALL_PIN, OUTPUT);
  
  
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
  //if (start % 100 == 0) {
  //  readMicrophone(); 
  //}
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

int lastTemperature = 0;

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
  String content = String("{\"volume\": 0, \"temperature\":" ) + lastTemperature + " }";
  http.POST(content);
}


void readMicrophone()
{
  unsigned int sample = analogRead(MICRO_PIN);
  Serial.println(sample);
}

