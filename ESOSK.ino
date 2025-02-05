#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(7, 8);  // RX, TX for Neo-6M GPS
SoftwareSerial gsmSerial(9, 10); // RX, TX for SIM800L GSM

const int buttonPin = 2; // Pin for the push button
const unsigned long requiredButtonPressDuration = 3000; // 3 seconds in milliseconds

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if (isButtonPressed()) {
    String gpsData = getGPSData();
    sendEmergencySMS(gpsData);
    delay(5000); // Avoid rapid SMS sending
    makeEmergencyCall();
  }
}

bool isButtonPressed() {
  if (digitalRead(buttonPin) == LOW) {
    unsigned long pressStartTime = millis();
    while (digitalRead(buttonPin) == LOW) {
      if (millis() - pressStartTime >= requiredButtonPressDuration) {
        return true;
      }
    }
  }
  return false;
}

String getGPSData() {
  unsigned long startMillis = millis();
  while (millis() - startMillis < 5000) { // Wait for 5 seconds to get a GPS fix
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isValid()) {
      return "Latitude: " + String(gps.location.lat(), 6) + ", Longitude: " + String(gps.location.lng(), 6);
    }
  }
  return "No GPS fix available.";
}

void sendEmergencySMS(String message) {
  gsmSerial.println("AT+CMGF=1"); // Set SMS to text mode
  delay(1000);
  gsmSerial.println("AT+CMGS=\"+123456789\""); // Replace with actual emergency number
  delay(1000);
  gsmSerial.print("Emergency! Location: " + message);
  delay(1000);
  gsmSerial.write(26); // Send Ctrl+Z to send SMS
  delay(1000);
}

void makeEmergencyCall() {
  gsmSerial.println("ATD+123456789;"); // Replace with actual emergency number
  delay(90000); // Wait for 90 seconds
  gsmSerial.println("ATH"); // Hang up call
}
