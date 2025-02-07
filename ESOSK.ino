#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pin definitions
const int buttonPin = 2; // Pin for the push button
const unsigned long requiredButtonPressDuration = 3000; // 3 seconds in milliseconds

// Serial communication for GPS and GSM
SoftwareSerial gpsSerial(7, 8);  // RX, TX for Neo-6M GPS
SoftwareSerial gsmSerial(9, 10); // RX, TX for SIM800L GSM

// GPS object
TinyGPSPlus gps;

// Button debounce variables
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce time in milliseconds

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);

  // Initialize button pin with internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Test GSM module
  if (!initializeGSM()) {
    Serial.println("GSM initialization failed. Check connections.");
  } else {
    Serial.println("GSM module initialized successfully.");
  }
}

void loop() {
  // Check if the button is pressed and held for the required duration
  if (isButtonPressed()) {
    Serial.println("Button pressed. Retrieving GPS data...");

    // Get GPS data
    String gpsData = getGPSData();
    if (gpsData != "No GPS fix available.") {
      Serial.println("GPS data retrieved: " + gpsData);

      // Send emergency SMS
      if (sendEmergencySMS(gpsData)) {
        Serial.println("Emergency SMS sent successfully.");
      } else {
        Serial.println("Failed to send emergency SMS.");
      }

      // Make emergency call
      if (makeEmergencyCall()) {
        Serial.println("Emergency call made successfully.");
      } else {
        Serial.println("Failed to make emergency call.");
      }
    } else {
      Serial.println("Failed to retrieve GPS data.");
    }

    // Delay to avoid rapid triggering
    delay(5000);
  }
}

bool isButtonPressed() {
  static bool buttonActive = false;
  static unsigned long pressStartTime = 0;

  // Read the button state
  if (digitalRead(buttonPin) == LOW) {
    if (!buttonActive) {
      buttonActive = true;
      pressStartTime = millis();
    } else if (millis() - pressStartTime >= requiredButtonPressDuration) {
      buttonActive = false; // Reset for next press
      return true;
    }
  } else {
    buttonActive = false; // Reset if button is released
  }

  return false;
}

String getGPSData() {
  unsigned long startMillis = millis();
  while (millis() - startMillis < 10000) { // Wait for 10 seconds to get a GPS fix
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isValid()) {
      return "Latitude: " + String(gps.location.lat(), 6) + ", Longitude: " + String(gps.location.lng(), 6);
    }
    delay(100); // Small delay to avoid busy-waiting
  }
  return "No GPS fix available.";
}

bool sendEmergencySMS(String message) {
  // Set SMS to text mode
  gsmSerial.println("AT+CMGF=1");
  if (!waitForResponse("OK", 1000)) return false;

  // Set the recipient number
  gsmSerial.println("AT+CMGS=\"+123456789\""); // Replace with actual emergency number
  if (!waitForResponse(">", 1000)) return false;

  // Send the emergency message
  gsmSerial.print("Emergency! Location: " + message);
  gsmSerial.write(26); // Send Ctrl+Z to send SMS

  // Wait for confirmation
  return waitForResponse("+CMGS:", 5000);
}

bool makeEmergencyCall() {
  // Dial the emergency number
  gsmSerial.println("ATD+123456789;"); // Replace with actual emergency number
  if (!waitForResponse("OK", 1000)) return false;

  // Wait for 90 seconds (call duration)
  delay(90000);

  // Hang up the call
  gsmSerial.println("ATH");
  return waitForResponse("OK", 1000);
}

bool initializeGSM() {
  // Check if the GSM module is responsive
  gsmSerial.println("AT");
  return waitForResponse("OK", 1000);
}

bool waitForResponse(const char* expectedResponse, unsigned long timeout) {
  unsigned long startMillis = millis();
  String response = "";

  while (millis() - startMillis < timeout) {
    while (gsmSerial.available() > 0) {
      char c = gsmSerial.read();
      response += c;
      if (response.indexOf(expectedResponse) != -1) {
        return true;
      }
    }
  }

  Serial.println("Timeout waiting for response: " + response);
  return false;
}
