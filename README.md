# GPS-GSM Based Emergency Alert System

## Overview
This project implements an emergency alert system using a GPS module (Neo-6M) and a GSM module (SIM800L). When a button is pressed for at least 3 seconds, the system retrieves the current GPS location and sends an SMS with the coordinates to a predefined emergency contact. It then initiates a call to the same contact.

## Hardware Requirements
- **Arduino (Uno/Nano/Compatible)**
- **Neo-6M GPS Module**
- **SIM800L GSM Module**
- **Push Button**
- **Connecting Wires**

## Circuit Connections
### GPS Module (Neo-6M)
- **VCC** → 5V
- **GND** → GND
- **TX** → Arduino Pin 7
- **RX** → Arduino Pin 8

### GSM Module (SIM800L)
- **VCC** → 5V (with appropriate power regulation)
- **GND** → GND
- **TX** → Arduino Pin 9
- **RX** → Arduino Pin 10

### Push Button
- One terminal → Arduino Pin 2
- Other terminal → GND

## Software Requirements
- **Arduino IDE** (for uploading the code)
- **TinyGPS++ Library** (for GPS data processing)
- **SoftwareSerial Library** (for communication with GPS and GSM modules)

## How It Works
1. The system continuously checks if the button is pressed.
2. If the button is held for **at least 3 seconds**, the GPS module fetches the current location.
3. The system sends an emergency SMS with GPS coordinates to a predefined number.
4. The system then places an emergency call to the same number.

## Code Explanation
- `isButtonPressed()`: Detects if the button is held for 3+ seconds.
- `getGPSData()`: Retrieves GPS coordinates within 5 seconds.
- `sendEmergencySMS()`: Sends an SMS with GPS location.
- `makeEmergencyCall()`: Initiates an emergency call.

## Notes
- Ensure the **GSM module** is properly powered (requires stable 4.2V-4.5V power supply).
- Check SIM card activation and network signal strength before deploying.
- Modify the predefined emergency contact number in the `sendEmergencySMS()` and `makeEmergencyCall()` functions.

## Future Enhancements (Planned)
- Implement additional safety features like **battery monitoring**.
- Add **an LCD/OLED display** to show system status.
- Integrate with **IoT cloud services** for remote monitoring.

## License
This project is open-source and available for modification and distribution under the MIT License.
