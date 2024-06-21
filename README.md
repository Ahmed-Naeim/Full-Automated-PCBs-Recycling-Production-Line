# Full-Automated-PCBs-Recycling-Production-Line

**Overview**

The Full Line Control System is a comprehensive industrial automation solution designed to manage and monitor a series of machines. It integrates local control via Arduino Mega with remote monitoring capabilities using ESP8266 and the Blynk platform. This system supports both automatic and manual operation modes, ensures safety through IR and ultrasonic sensors, and provides real-time status updates through an OLED display and the Blynk mobile app.


**Features**

Automatic Mode: Sequential control of machines based on predefined sequences.
Manual Mode: Direct control over individual machines via a keypad interface.
Safety Sensors: IR and ultrasonic sensors detect motion and obstacles, ensuring workplace safety.
User Interface: Keypad and OLED display provide interaction and display system status.
Remote Monitoring: Integration with Blynk allows real-time monitoring and control from anywhere.

**Components**

Arduino Mega: Controls relay modules for machine operation and interfaces with sensors and user inputs.
ESP8266: Facilitates communication with the Blynk platform for remote monitoring and control.
IR and Ultrasonic Sensors: Ensures safety by detecting motion and obstacles in the workspace.
OLED Display: Provides real-time status updates and prompts for user interaction.
Keypad: Allows selection of operation modes (automatic/manual) and input of access credentials.

**Installation and Usage**

**Hardware Setup**
Connect Arduino Mega:
Wire Arduino Mega to relay modules, IR and ultrasonic sensors, OLED display, and keypad.
Connect ESP8266:
Establish communication between ESP8266 and Arduino Mega for Blynk integration.
**Software Setup**
Upload Arduino Mega Firmware:

Flash arduino_code.ino to manage machine operations, interface with sensors, and handle user inputs.
Upload ESP8266 Firmware:

Upload esp8266_code.ino to enable communication with Blynk, transmit system status, and receive control commands.

**Integration with Blynk**

Configure Blynk App:
Obtain Blynk authentication token and Wi-Fi credentials.
Set up virtual pins in the Blynk app for monitoring machine states and controlling operation modes remotely.

**Documentation**

Detailed Documentation: See Documentation for comprehensive information on system architecture, component connections, operational modes, safety protocols, and Blynk integration.
Demonstration Video: Watch our demonstration video to see installation steps, system operation, and user interaction scenarios.
**Future Enhancements
**Implement predictive maintenance using sensor data analytics.
Enhance user interface for improved user experience and accessibility.
Integrate with cloud platforms for data logging and advanced analytics.

**Contributors**

Ahmed Mohamed Naeim - Project Lead & Developer

**License**

This project is licensed under the MIT License.


