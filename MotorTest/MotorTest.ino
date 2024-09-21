#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* serverName = "mongodb+srv://kashyapmistry2021:ws7Gqbfgy3*hQZ5@db1cluster1.skf8r.mongodb.net/?retryWrites=true&w=majority&appName=DB1Cluster1";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");

  HTTPClient http;
  http.begin(serverName);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Response:");
    Serial.println(payload);

    // Parse JSON
    DynamicJsonDocument doc(1024); // Adjust size as needed
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Assuming the JSON response is an array of rides
    for (JsonObject ride : doc.as<JsonArray>()) {
      String name = ride["name"].as<String>();
      String pickup = ride["pickup"].as<String>();
      String destination = ride["destination"].as<String>();

      Serial.println("Ride:");
      Serial.print("Name: ");
      Serial.println(name);
      Serial.print("Pickup: ");
      Serial.println(pickup);
      Serial.print("Destination: ");
      Serial.println(destination);
    }

  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}

void loop() {
  // Empty loop
}
