#define BLYNK_TEMPLATE_ID "TMPL3RR8REVUt"
#define BLYNK_TEMPLATE_NAME "Soil Moisture and Temperature Monitor with ESP32"
#define BLYNK_AUTH_TOKEN "adYIos2iD8F3-AtdgO8yg31Q8ip0_F3W"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <HTTPClient.h>

// WiFi credentials
char ssid[] = "Kamireddy";
char pass[] = "reddy1977";
char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

#define DHTPIN 4
#define SOIL_MOISTURE_PIN 34
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ThingSpeak API Key (replace this with your actual Write API Key)
String thingSpeakApiKey = "N4F7EVLNEV9LPUCA";  // <-- put your API key here

void sendSensor() {
  int soilmoisture = analogRead(SOIL_MOISTURE_PIN);
  int soilmoisturepercentage = map(soilmoisture, 3500, 4095, 100, 0);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send to Blynk
  Blynk.virtualWrite(V0, soilmoisturepercentage);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);

  Serial.print("Soil Moisture : ");
  Serial.print(soilmoisturepercentage);
  Serial.print("%  Temperature : ");
  Serial.print(t);
  Serial.print("°C  Humidity : ");
  Serial.println(h);

  // Send to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key=" + thingSpeakApiKey +
                 "&field1=" + String(soilmoisturepercentage) +
                 "&field2=" + String(t) +
                 "&field3=" + String(h);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.print("ThingSpeak Response Code: ");
      Serial.println(httpCode);
    } else {
      Serial.print("Error sending to ThingSpeak: ");
      Serial.println(http.errorToString(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Not Connected");
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(15000L, sendSensor); // 15 sec interval
}

void loop() {
  Blynk.run();
  timer.run();
}
