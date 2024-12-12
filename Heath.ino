#define BLYNK_TEMPLATE_ID "TMPL2nSgSQbRI"
#define BLYNK_TEMPLATE_NAME "Health Monitoring"
#define BLYNK_AUTH_TOKEN "DYo1I7nKM88UucufvHkh7uSX-frZDOhK"
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include "DHT.h"

// DHT11 
#define DHTTYPE DHT11
#define DHTPin 33
DHT dht(DHTPin, DHTTYPE);

// Pulse sensor 
#define PulsePin 34  
volatile int pulseRate = 0;
volatile bool pulseDetected = false;
unsigned long lastBeatTime = 0;
int beatInterval = 0;

// WiFi and Blynk credentials
char auth[] = "DYo1I7nKM88UucufvHkh7uSX-frZDOhK";  // Blynk Auth Token
char ssid[] = "TTT";             // ssid
char pass[] = "12345678";         // pw

float temperature = 0.0, humidity = 0.0;

#define REPORTING_PERIOD_MS 1000
unsigned long tsLastReport = 0;

void IRAM_ATTR onPulseDetected() {
    unsigned long currentTime = millis();
    beatInterval = currentTime - lastBeatTime;

    if (beatInterval > 250) {  
        pulseRate = 60000 / beatInterval;
        pulseDetected = true;
        lastBeatTime = currentTime;
    }
}

void setup() {
    Serial.begin(115200);//baud rate 
    pinMode(PulsePin, INPUT);
    attachInterrupt(digitalPinToInterrupt(PulsePin), onPulseDetected, RISING);

    dht.begin();

    Blynk.begin(auth, ssid, pass);
    Serial.println("System Initialized");
}

void loop() {
    Blynk.run();

    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.println("\n--- Sensor Readings ---");

        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");

        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");

        if (pulseDetected) {
            Serial.print("Heart Rate: ");
            Serial.print(pulseRate);
            Serial.println(" bpm");
            pulseDetected = false;  // Reset 
        } else {
            Serial.println("Heart Rate: No pulse detected");
        }
       //Data to blynk
        Blynk.virtualWrite(V1, temperature);  
        Blynk.virtualWrite(V2, humidity);     
        Blynk.virtualWrite(V3, pulseRate);  

      
        tsLastReport = millis();
    }
}