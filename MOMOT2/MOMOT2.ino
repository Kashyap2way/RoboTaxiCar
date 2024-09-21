#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>  // For handling time synchronization

// PCA9685 setup
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
const int escChannel1 = 0;  // ESC connected to channel 0
const int escChannel2 = 1;
const int minPulseWidth = 600;  // 600 µs corresponds to minimum throttle
const int maxPulseWidth = 2500;  // 2500 µs corresponds to maximum throttle

// WiFi and HTTP setup
const char* ssid = "JassKaNet";
const char* password = "connect mat karo";
const char* serverName = "https://esp32-to-mongodb.onrender.com/rides";

// NTP time synchronization setup
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;  // GMT+5:30 (Indian Standard Time)
const int daylightOffset_sec = 0;  // No daylight saving time

// Variables to keep track of latest data
String latestDataId = "";  // To track the ID of the latest ride
time_t latestTimestamp = 0;  // To track the latest timestamp

// Timer variables
bool escRunning = false;
unsigned long startTime = 0;
const unsigned long runDuration = 10000;  // 10 seconds in milliseconds

void setup() {
  Serial.begin(115200);

  // Initialize PCA9685 and set PWM frequency to 50 Hz for ESC control
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz for servo/ESCs
  delay(10);  // Allow PCA9685 to initialize

  // Arm ESCs by sending minimum throttle signal
  Serial.println("Starting ESC calibration for 2 sec");
  setESC(escChannel1, minPulseWidth);
  setESC(escChannel2, minPulseWidth);
  delay(2000);  // Give ESCs time to arm

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  // Lower delay to check WiFi status more frequently
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Synchronize time with NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    Serial.println("Time synchronized");
  }

  // Fetch and process data from the server
  fetchAndProcessData();
}

void loop() {
  // Continuously fetch and process data with a very short interval
  fetchAndProcessData();
  delay(1000);  // Reduce delay to check for data every 1 second for faster response

  // Check if ESCs have been running for 10 seconds
  if (escRunning && (millis() - startTime >= runDuration)) {
    stopESCs();
    escRunning = false;
  }
}

// Function to fetch and process data from the server
void fetchAndProcessData() {
  HTTPClient http;
  http.setTimeout(5000);  // Lower timeout to 5 seconds for faster failure detection
  http.begin(serverName);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Response:");
    Serial.println(payload);  // Print the actual payload

    // Parse JSON response
    DynamicJsonDocument doc(4096);  // Adjust size as needed
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Flag to determine if the latest data is found
    bool newDataFound = false;

    // Get current system time
    time_t now;
    time(&now);

    // Process JSON array
    for (JsonObject ride : doc.as<JsonArray>()) {
      String id = ride["_id"].as<String>();  // Assuming data has a unique '_id' field
      String dateTimeStr = ride["dateTime"].as<String>();  // Using the "dateTime" field

      // Convert the dateTime (YYYY/MM/DD HH:MM:SS) to time_t using strptime
      struct tm tm = {0};
      if (strptime(dateTimeStr.c_str(), "%Y/%m/%d %H:%M:%S", &tm) != NULL) {
        time_t dataTimestamp = mktime(&tm);

        // Compare data timestamp with the latest timestamp and system time
        if (difftime(dataTimestamp, latestTimestamp) > 0 && difftime(now, dataTimestamp) < 120) {  // Only if data is newer and less than 2 minutes old
          String name = ride["name"].as<String>();
          String pickup = ride["pickup"].as<String>();
          String destination = ride["destination"].as<String>();

          Serial.println("New Ride Data:");
          Serial.print("Name: ");
          Serial.println(name);
          Serial.print("Pickup: ");
          Serial.println(pickup);
          Serial.print("Destination: ");
          Serial.println(destination);
          Serial.print("DateTime: ");
          Serial.println(dateTimeStr);

          // Update latestDataId and latestTimestamp to the new data's ID and timestamp
          latestDataId = id;
          latestTimestamp = dataTimestamp;

          // Immediately start ESCs based on the new data
          Serial.println("Starting ESCs at 70% throttle");
          startESCs();  // Start ESCs and start the timer

          newDataFound = true;
          return;  // Exit function immediately after processing new data
        }
      } else {
        Serial.println("Failed to parse dateTime");
      }
    }

    if (!newDataFound) {
      Serial.println("No new data found or data is too old.");
    }

  } else {
    Serial.print("Error on HTTP request. Code: ");
    Serial.println(httpCode);
    Serial.print("Error Message: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();
}

// Function to convert percentage to pulse width in microseconds
int calculatePulseWidth(int percent) {
  return map(percent, 0, 100, minPulseWidth, maxPulseWidth);
}

// Function to start the ESCs and begin the timer
void startESCs() {
  setESC(escChannel1, calculatePulseWidth(70));  // Set to 70% throttle
  setESC(escChannel2, calculatePulseWidth(70));  // Set to 70% throttle
  startTime = millis();  // Start timer
  escRunning = true;
}

// Function to stop the ESCs after 10 seconds
void stopESCs() {
  Serial.println("Stopping ESCs");
  setESC(escChannel1, minPulseWidth);  // Set to minimum throttle to stop
  setESC(escChannel2, minPulseWidth);  // Set to minimum throttle to stop
}

// Function to send PWM signal to ESC
void setESC(int channel, int pulseWidth) {
  int pulseLength = map(pulseWidth, 0, 20000, 0, 4095);  // Convert to 12-bit value for PCA9685
  pwm.setPWM(channel, 0, pulseLength);
}
