#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi network credentials
const char* ssid = "data";
const char* password = "datadata";

// Pin number for GPIO6
const int gpio6Pin = 7;

// Pin number for GPIO7
const int gpio7Pin = 6;

// Timer interval in milliseconds
const unsigned long timerInterval = 30000;

// Variable to store the last timer value
unsigned long lastTimerMillis = 0;

// Flag to track the current pin state
bool isGpio6On = true;

void setup() {
  // Set GPIO6 as an output
  pinMode(gpio6Pin, OUTPUT);
  // Set GPIO7 as an output
  pinMode(gpio7Pin, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Initialize serial communication
  Serial.begin(115200);
}

void loop() {
  // Check if the timer interval has elapsed
  if (millis() - lastTimerMillis >= timerInterval) {
    // Make an API request
    bool result = makeAPIRequest();
  
    // Toggle the state of the GPIO pins based on the API response
    if (result) {
      digitalWrite(gpio6Pin, HIGH); // Turn on GPIO6
      digitalWrite(gpio7Pin, LOW);  // Turn off GPIO7
    } else {
      digitalWrite(gpio6Pin, LOW);  // Turn off GPIO6
      digitalWrite(gpio7Pin, HIGH); // Turn on GPIO7
    }
  
    // Update the last timer value
    lastTimerMillis = millis();
  }
}

bool makeAPIRequest() {
  // Create an HTTPClient object
  HTTPClient http;

  // Specify the API endpoint URL
  String apiUrl = "https://thedudes-seven.vercel.app/api";

  // Send a GET request to the API endpoint
  int httpCode = http.begin(apiUrl);
  httpCode = http.GET();

  // Check the HTTP response code
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Parse the JSON response
    StaticJsonDocument<256> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, payload);

    // Check for parsing errors
    if (error) {
      Serial.println("JSON Parsing failed: " + String(error.c_str()));
      return false;
    }

    // Retrieve the value of the "result" field
    bool result = jsonDoc["result"];

    // Return the result
    return result;
  } else {
    Serial.println("API Request failed with error code: " + String(httpCode));
    return false;
  }

  // Close the HTTP connection
  http.end();
}
